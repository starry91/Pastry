#include "clientDatabase.h"
#include "errorMsg.h"
#include <tgmath.h>
#include <vector>
using namespace std;

ClientDatabase::ClientDatabase()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->row = ceil((log((N)) * 1.000) / log(pow(2, b)));
	this->col = pow(2, (b));
	this->routingTable = vector<vector<node_Sptr>>(this->row, vector<node_Sptr>(this->col));
	this->recieved_update_count = 0;
	this->total_route_length = this->row;
};

ClientDatabase &ClientDatabase::getInstance()
{
	static ClientDatabase res;
	return res;
}

node_Sptr ClientDatabase::getNextRoutingNode(string nodeID)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &left_leafSet = this->leafSet.first;
	auto &right_leafSet = this->leafSet.second;
	auto left_most_leaf = (*left_leafSet.begin())->getNodeID();
	auto right_most_leaf = (*right_leafSet.begin())->getNodeID();
	if (nodeID > left_most_leaf and nodeID < right_most_leaf)
	{
		/// Next Routing Entry is in leaf set
		auto closest_node = *left_leafSet.begin();
		for (auto node : left_leafSet)
		{
			if (is_better_node(node, closest_node, nodeID))
			{
				closest_node = node;
			}
		}
		for (auto node : left_leafSet)
		{
			if (is_better_node(node, closest_node, nodeID))
			{
				closest_node = node;
			}
		}
		return closest_node;
	}
	auto prefix = prefixMatchLen(nodeID, this->listener->getNodeID());
	if (this->routingTable[prefix][nodeID[prefix] - '0'])
	{
		return routingTable[prefix][nodeID[prefix] - '0'];
	}
	auto closest_node = *left_leafSet.begin();
	for (auto node : left_leafSet)
	{
		if (is_better_node(node, closest_node, nodeID))
		{
			closest_node = node;
		}
	}
	for (auto node : left_leafSet)
	{
		if (is_better_node(node, closest_node, nodeID))
		{
			closest_node = node;
		}
	}
	for (auto routingRow : routingTable)
	{
		for (auto node : routingRow)
		{
			if (is_better_node(node, closest_node, nodeID))
			{
				closest_node = node;
			}
		}
	}
	for (auto node : neighbourSet)
	{
		if (is_better_node(node, closest_node, nodeID))
		{
			closest_node = node;
		}
	}
	return closest_node;
}

vector<vector<node_Sptr>> ClientDatabase ::getRoutingTable()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->routingTable;
}

pair<set<node_Sptr, leafComparator>, set<node_Sptr, leafComparator>> ClientDatabase ::getLeafSet()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->leafSet;
}

set<node_Sptr, neighbourComparator> ClientDatabase ::getNeighbourSet()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->neighbourSet;
}

void ClientDatabase::addToLeafSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (node->getNodeID() < this->listener->getNodeID())
	{
		auto &left_leafSet = this->leafSet.first;
		left_leafSet.insert(node);
		if (left_leafSet.size() > this->col / 2)
		{
			left_leafSet.erase(left_leafSet.begin());
		}
	}
	else
	{
		auto &right_leafSet = this->leafSet.second;
		right_leafSet.insert(node);
		if (right_leafSet.size() > this->col / 2)
		{
			right_leafSet.erase(*right_leafSet.rbegin());
		}
	}
	return;
}

void ClientDatabase::addToNeighhbourSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &neighbour = this->neighbourSet;
	neighbour.insert(node);
	if (neighbour.size() > col)
	{
		neighbour.erase(*neighbour.rbegin());
	}
	return;
}

void ClientDatabase ::addToRoutingTable(node_Sptr node, int prefix = -1)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (prefix == -1)
	{
		prefix = prefixMatchLen(this->listener->getNodeID(), node->getNodeID());
	}
	auto index = node->getNodeID()[prefix] - '0';
	if (!this->routingTable[prefix][index])
	{
		this->routingTable[prefix][index] = node;
		return;
	}
	if (node->getProximity() < this->routingTable[prefix][index]->getProximity())
	{
		this->routingTable[prefix][index] = node;
	}
}
void ClientDatabase ::updateAllState(node_Sptr node)
{
	this->addToLeafSet(node);
	this->addToNeighhbourSet(node);
	this->addToRoutingTable(node);
	return;
}

void ClientDatabase ::updateRoutingTable(vector<node_Sptr> row_entry, int index)
{
	for (auto node : row_entry)
	{
		this->addToRoutingTable(node, index);
	}
	return;
}

int ClientDatabase::getRowSize()
{
	return this->row;
}

int ClientDatabase::getColSize()
{
	return this->col;
}

void ClientDatabase::setTotalRouteLength(int s)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->total_route_length = s;
}
void ClientDatabase::incrementRecievedUpdateCount(int n = 1)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->total_route_length + n;
}
int ClientDatabase::getRecievedUpdateCount()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->recieved_update_count;
}
void ClientDatabase::resetUpdateValues()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->total_route_length = this->row;
	this->recieved_update_count = 0;
}