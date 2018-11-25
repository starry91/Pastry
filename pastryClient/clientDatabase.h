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
#include <unordered_map>
#include <thread>
#include "peerCommunicator.h"
//Pastry parameters
#define config_parameter_b 1
#define parameter_N 1000

class ClientDatabase
{
  private:
	std::mutex seeder_mtx; // mutex for critical section
	std::mutex shutdown_mtx; // mutex for critical section
	int row;
	int col;
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> leafSet;
	std::vector<std::vector<node_Sptr>> routingTable;
	std::set<node_Sptr, neighbourComparator> neighbourSet;
	node_Sptr listener;
	int total_route_length;
	int recieved_update_count;
	// std :: thread :: id listener_thread_id;
	std::unordered_map<std ::string, std::string> hashMap;
	bool is_same_node_as_me(node_Sptr node);
	ClientDatabase();

  public:
	static ClientDatabase &getInstance();
	void setListener(node_Sptr);
	node_Sptr getListener();
	node_Sptr getNextRoutingNode(std::string nodeID);
	std::vector<std::vector<node_Sptr>> getRoutingTable();
	std::pair<std::set<node_Sptr, leafComparator>, std::set<node_Sptr, leafComparator>> getLeafSet();
	std::set<node_Sptr, neighbourComparator> getNeighbourSet();
	void addToNeighhbourSet(node_Sptr node); // add this node to Neighbour set
	void addToLeafSet(node_Sptr node);		 // add this node to leaf set
	void addToRoutingTable(node_Sptr node, int = -1);
	void deleteFromNeighhbourSet(node_Sptr node); // add this node to Neighbour set
	void deleteFromLeafSet(node_Sptr node);		  // add this node to leaf set
	void deleteFromRoutingTable(node_Sptr node);
	void updateAllState(node_Sptr node);								  // give node pointer for updating it in table
	void updateRoutingTable(std::vector<node_Sptr> row_entry, int index); //give roww and index
	int getRowSize();
	int getColSize();
	void setTotalRouteLength(int s);
	int getTotalRouteLength();
	void incrementRecievedUpdateCount(int = 1);
	int getRecievedUpdateCount();
	void resetUpdateValues();
	void insertIntoHashMap(std ::string key, std ::string value);
	std::unordered_map<std ::string, std::string> getHashMap();
	std::string getHashMapValue(std ::string key);
	// void setListenerThreadID(std::thread::id thread_id);
	// std::thread::id getListenerThreadID();
	void deleteFromHashMap(std::pair<std::string, std::string> entry_to_delete);
	void delete_from_all(node_Sptr node);
	node_Sptr findInLeafSet(std::set<node_Sptr, leafComparator> &, std::string nodeId);
	node_Sptr findInNeighourSet(std::set<node_Sptr, neighbourComparator> &, std::string nodeId);
	int findInLeafSet(node_Sptr node);
	node_Sptr findInNeighourSet(node_Sptr node);
	std::pair<int, int> findInRoutingTable(node_Sptr node);
	void lazyUpdateLeafSet(bool leaf_set_side);
	void lazyUpdateNeighbourSet();
	void lazyUpdateRoutingTable(std::pair<int, int> position);
	void lockShutdown();
	void unlockShutdown();
};
#endif