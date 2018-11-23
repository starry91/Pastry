#include "clientDatabase.h"
#include "errorMsg.h"
#include <tgmath.h>
#include <vector>
using namespace std;

ClientDatabase::ClientDatabase()
{
	this->row = ceil((log((N)) * 1.000) / log(pow(2, b)));
	this->col = pow(2, (b)) - 1;
	this->routingTable = vector<vector<node_Sptr>>(this->row, vector<node_Sptr>(this->col));
};

ClientDatabase &ClientDatabase::getInstance()
{
	static ClientDatabase res;
	return res;
}

bool is_better_node(node_Sptr node1, node_Sptr node2, string nodeID){
	auto node1_nodeID = node1->getNodeID();
	auto node2_nodeID = node2->getNodeID();
	for ( auto i=0;i<nodeID.length();i++){
		if(node1_nodeID[i] != node2_nodeID[i]){
			return abs(node1_nodeID[i] - nodeID[i]) < abs(node2_nodeID[i] - nodeID[i]);
		}
	}
	return false;
}

node_Sptr ClientDatabase:: getNextRoutingNode(string nodeID)
{
	auto &left_leafSet = this->leafSet.first;
	auto &right_leafSet = this->leafSet.second;
	auto left_most_leaf = (*left_leafSet.begin())->getNodeID();
	auto right_most_leaf = (*right_leafSet.begin())->getNodeID();
	if(nodeID > left_most_leaf and nodeID < right_most_leaf){
		/// Next Routing Entry is in leaf set
		auto closest_node = *left_leafSet.begin();
		for(auto node: left_leafSet){
			if(is_better_node(node, closest_node, nodeID)){
				closest_node = node;
			}
		}
		for(auto node: left_leafSet){
			if(is_better_node(node, closest_node, nodeID)){
				closest_node = node;
			}
		}
		return closest_node;
	}
	auto prefix = ;
	
}
