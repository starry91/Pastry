//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef CLIENTDATABASE_H
#define CLIENTDATABASE_H

#include <string>
#include <iostream>
#include <map>
#include "node.h"
#include <mutex>
#include <condition_variable>
#include <set>
#include "utils.h"
#include <vector>

//Pastry parameters
#define b 8
#define N 10000

class ClientDatabase
{
  private:
	int row;
	int col;
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> leafSet;
	std::vector<std::vector<node_Sptr>> routingTable;
	std::set<node_Sptr, neighbourComparator> neighbourSet;
	node_Sptr listener;
	ClientDatabase();

  public:
	static ClientDatabase &getInstance();
	void setListener(Node);
	node_Sptr getListener();
	node_Sptr getNextRoutingNode(std::string nodeID);
	std::vector<std::vector<node_Sptr>> getRoutingTable();
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> getLeafSet();
	std::set<node_Sptr, neighbourComparator> getNeighbourSet();
	bool is_better_node(node_Sptr node1, node_Sptr node2, string nodeID); //is node1 more closer to nodeID than node2
	void addToNeighhbourSet(node_Sptr node);
	void addToLeafSet(node_Sptr node);
};
#endif