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

class ClientDatabase
{
private:
  int b;
  int N;
  int row;
  int col;
  std::pair<set<node_Sptr, leafComparator>,set<node_Sptr, leafComparator>> leafSet;
  vector<vector<node_Sptr>> routingTable;
  set<node_Sptr, neighbourComparator> neighbourSet;
  node_Sptr listener;
  ClientDatabase();

public:
  static ClientDatabase &getInstance();
  void setListener(Node);
  node_Sptr getListener();
  node_Sptr getNextRoutingNode(string nodeID);
  vector<vector<node_Sptr>> getRoutingTable();
  std::pair<set<node_Sptr, leafComparator>,set<node_Sptr, leafComparator>> getLeafSet();
  set<node_Sptr, neighbourComparator> getNeighbourSet();
};

#endif