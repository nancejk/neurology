//Defines Neurons (the things that live at the vertices) and
//Conduits, which connect them.  Then typedefs NNet::Network
//as a boost graph of this sort.

#include <boost/graph/adjacency_list.hpp>	//Defines the adjacency_list

//The Neural Net (NNet) namespace in which everything will be defined.
namespace NNet
{
	//The computational units that sit at the vertices of the graph.
	struct Neuron
	{
		//The value that must be present in total at the in-edges of
		//a neuron in order for it to fire.
		double threshold;
		
		//The value that a neuron above threshold adds to its out-edges.
		double strength;
		
		//A measure of distance from the head nodes.  Each 'tier' is a hop
		//away from the head.
		std::size_t tier;
	};
	
	//The connectivity units that connect one Neuron to another.
	struct Conduit
	{
		//The value that has currently been carried along the network.
		double pressure;
		
		//A carrying capacity for the conduit.  Signals arriving at the
		//conduit will saturate at this level.
		double capacity;
	};

	typedef boost::adjacency_list<boost::vecS,
								boost::vecS,
								boost::bidirectionalS,
								NNet::Neuron,
								NNet::Conduit> Network;

	//A special iterator for Networks.
	typedef boost::graph_traits<Network>::vertex_iterator net_walker;
	
	//A function that returns a Network object.
	Network BuildNewNetwork(unsigned numNodes, unsigned minOut, unsigned maxOut);
}//NNet



