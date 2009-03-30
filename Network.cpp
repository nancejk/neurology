#include <numeric> //For std::accumulate
#include <neurology/Network.hpp>
#include <randomnumberengine/RandomNumberEngine.h>

namespace NNet
{
	struct unique_number
	{
		unsigned current;
		unique_number() { current = 0; };
		unsigned operator()() { return ++current; };	
	} Unique;
	
	Network BuildNewNetwork(unsigned numNodes, unsigned minOut, unsigned maxOut)
	{
		using NNet::Network;
		
		//Use a random number engine to set the thresholds.
		RandomNumberEngine theRNE;
		
		//This is the 'input complexity', or number of input neurons. 
		static const std::size_t inCpx(1);
		//this is the 'output complexity' which is the number of output neurons.
		static const std::size_t outCpx(1);
		
		//This is a ballpark guess at the size that we want our network to be.
		static const std::size_t ballpark = numNodes;
		
		//The boundaries of the layers.  boundaries[i] is the end of the i'th layer.
		std::vector<unsigned> boundaries(1);
		boundaries[0] = inCpx;
		
		//Now we can generate the layers by recursively adding boundaries to the
		//boundary list.  Details in the header.
		while ( std::accumulate(boundaries.begin(), boundaries.end(), 0) < ballpark )
		{
			boundaries.push_back( boundaries.back() + theRNE.GetUniformRandomInt( minOut*boundaries.back(), maxOut*boundaries.back() ) );
			std::cout << "New layer with " << boundaries.back() << " nodes created." << std::endl;
		}
		
		//Now push on the output layer.
		boundaries.push_back( boundaries.back() + outCpx );
		
		//Now build the nodeID vector.
		std::vector<unsigned> nodeID( std::accumulate(boundaries.begin(), boundaries.end(), 0) );
		std::generate( nodeID.begin(), nodeID.end(), Unique );
		
		//With the nodeIDs and the boundaries, we can construct the sets that are
		//the nodeIDs for each individual layer.
		std::vector<std::set<unsigned> >layer;
		std::size_t running_count(0);
		for ( std::size_t i = 0; i < boundaries.size(); i ++ )
		{
			layer.push_back( std::set<unsigned>(nodeID.begin()+running_count,nodeID.begin()+boundaries[i]) );
			running_count = boundaries[i]; 
		}
		
		//Now we have sets which give us the IDs of the nodes in each layer.  Now
		//we just randomly connect them.  Iterate through all of the layers, and in
		//each one, randomly pick the nodes at the next layer to join to.  Add these
		//to the edgelist.
		typedef std::pair<unsigned,unsigned> Edge;
		std::vector<Edge> edge_list;
		
		typedef std::vector<std::set<unsigned> > layerlist;
		for ( layerlist::iterator l_it = layer.begin(); l_it != layer.end() - 2; l_it++ )
		{
			//Now iterate through the nodes in the current layer and randomly connect
			//to the nodes in the next layer.
			for ( std::set<unsigned>::iterator layer_it = l_it->begin(); layer_it != l_it->end(); layer_it++ )
			{
				//Make a temporary copy of the nodeIDs ahead.
				std::set<unsigned> forwardIDs( *(l_it+1) );
				
				//We have to connect to at least the minimum, and maybe more.
				std::size_t connections = theRNE.GetUniformRandomInt( minOut, forwardIDs.size() );
				for ( std::size_t conn_queue = 0; conn_queue < connections; conn_queue++)
				{
					unsigned outEnd = theRNE.GetUniformRandomInt( *forwardIDs.begin(), *forwardIDs.rbegin() );
					edge_list.push_back( Edge(*layer_it, outEnd) );
					forwardIDs.erase( outEnd );
				}
			}
		}
		return Network(edge_list.begin(),edge_list.end(),*std::max_element(nodeID.begin(),nodeID.end()) );
	}
}	//NNet