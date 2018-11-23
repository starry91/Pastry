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
  	std::mutex seeder_mtx; // mutex for critical section
	int row;
	int col;
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> leafSet;
	std::vector<std::vector<node_Sptr>> routingTable;
	std::set<node_Sptr, neighbourComparator> neighbourSet;
	node_Sptr listener;
	int total_route_length;
	int recieved_update_count;
	ClientDatabase();

  public:
	static ClientDatabase &getInstance();
	void setListener(Node);
	node_Sptr getListener();
	node_Sptr getNextRoutingNode(std::string nodeID);
	std::vector<std::vector<node_Sptr>> getRoutingTable();
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> getLeafSet();
	std::set<node_Sptr, neighbourComparator> getNeighbourSet();s
	void addToNeighhbourSet(node_Sptr node);							  // add this node to Neighbour set
	void addToLeafSet(node_Sptr node);									  // add this node to leaf set
	void addToRoutingTable(node_Sptr node, int prefix = -1);
	void updateAllState(node_Sptr node);							 // give node pointer for updating it in table
	void updateRoutingTable(vector<node_Sptr> row_entry, int index); //give roww and index
	int getRowSize();
	int getColSize();
	void setTotalRouteLength(int s);
	void incrementRecievedUpdateCount(int n = 1);
	int getRecievedUpdateCount();
	void resetUpdateValues();
};
#endif