#include "clientDatabase.h"
#include "errorMsg.h"
#include <tgmath.h>
#include <vector>
#include <syslog.h>
#include "logHandler.h"
using namespace std;

ClientDatabase::ClientDatabase()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->row = ceil((log((parameter_N)) * 1.000) / log(pow(2, config_parameter_b)));
	this->col = pow(2, (config_parameter_b));
	this->routingTable = vector<vector<node_Sptr>>(this->row, vector<node_Sptr>(this->col, nullptr));
	this->recieved_update_count = 0;
	this->total_route_length = this->row;
}

ClientDatabase &ClientDatabase::getInstance()
{
	static ClientDatabase res;
	return res;
}

node_Sptr ClientDatabase::getListener()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->listener;
}

void ClientDatabase::setListener(node_Sptr temp)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->listener = temp;
	return;
}

node_Sptr ClientDatabase::getNextRoutingNode(string nodeID)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &left_leafSet = this->leafSet.first;
	auto &right_leafSet = this->leafSet.second;
	auto left_most_leaf = this->listener->getNodeID();
	if (!left_leafSet.empty())
	{
		left_most_leaf = (*left_leafSet.begin())->getNodeID();
	}
	auto right_most_leaf = this->listener->getNodeID();
	if (!right_leafSet.empty())
	{
		right_most_leaf = (*right_leafSet.begin())->getNodeID();
	}
	if (!left_leafSet.empty() or !right_leafSet.empty())
	{
		if (nodeID >= left_most_leaf and nodeID <= right_most_leaf)
		{
			/// Next Routing Entry is in leaf set
			auto closest_node = this->listener;
			for (auto node : left_leafSet)
			{
				if (is_better_node(node, closest_node, nodeID))
				{
					closest_node = node;
				}
			}
			for (auto node : right_leafSet)
			{
				if (is_better_node(node, closest_node, nodeID))
				{
					closest_node = node;
				}
			}
			std::string log_msg = "node chosen from ip : " + closest_node->getIp() + " port : " + closest_node->getPort();
			LogHandler::getInstance().logMsg(log_msg);
			return closest_node;
		}
	}
	auto prefix = prefixMatchLen(nodeID, this->listener->getNodeID());
	if (this->routingTable[prefix][nodeID[prefix] - '0'])
	{
		std::string log_msg = "node chosen from ROUTING TABLE IP: " + this->routingTable[prefix][nodeID[prefix] - '0']->getIp() +
							  "Port: " + this->routingTable[prefix][nodeID[prefix] - '0']->getPort();
		LogHandler::getInstance().logMsg(log_msg);
		return routingTable[prefix][nodeID[prefix] - '0'];
	}
	auto closest_node = this->listener;
	for (auto node : left_leafSet)
	{
		if (is_better_node(node, closest_node, nodeID))
		{
			closest_node = node;
		}
	}
	for (auto node : right_leafSet)
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
	string log_msg = "Next best route IP: " + closest_node->getIp() + " Port: " + closest_node->getPort() + " NodeID: " + closest_node->getNodeID();
	LogHandler::getInstance().logMsg(log_msg);
	return closest_node;
}

vector<vector<node_Sptr>> ClientDatabase::getRoutingTable()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->routingTable;
}

pair<set<node_Sptr, leafComparator>, set<node_Sptr, leafComparator>> ClientDatabase::getLeafSet()
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
	// cout << "Entering in addToLeaf" << endl;
	if (!node)
	{
		return;
	}
	seeder_mtx.lock();
	if (this->is_same_node_as_me(node))
	{
		return;
	}
	seeder_mtx.unlock();

	int proximity = calculateProximity(node->getIp());
	node->setProximity(proximity);

	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (node->getNodeID() < this->listener->getNodeID())
	{
		auto &left_leafSet = this->leafSet.first;
		auto leaf_entry = this->findInLeafSet(left_leafSet, node->getNodeID());
		if (!leaf_entry)
		{
			left_leafSet.insert(node);
			std::string log_msg = "adding to left leaf set IP : " + node->getIp() + " Port : " + node->getPort() + " NodeID: " + node->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
		}
		if (left_leafSet.size() > this->col / 2)
		{
			left_leafSet.erase(left_leafSet.begin());
		}
	}
	else
	{
		auto &right_leafSet = this->leafSet.second;
		if (!this->findInLeafSet(right_leafSet, node->getNodeID()))
		{
			right_leafSet.insert(node);
			std::string log_msg = "adding to right leaf set IP : " + node->getIp() + " Port : " + node->getPort() + " NodeID: " + node->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
		}
		if (right_leafSet.size() > this->col / 2)
		{
			right_leafSet.erase(*right_leafSet.rbegin());
		}
	}
	return;
}

void ClientDatabase::addToNeighhbourSet(node_Sptr node)
{
	if (!node)
	{
		return;
	}
	seeder_mtx.lock();
	if (this->is_same_node_as_me(node))
	{
		return;
	}
	seeder_mtx.unlock();

	int proximity = calculateProximity(node->getIp());
	node->setProximity(proximity);

	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &neighbour = this->neighbourSet;
	auto n_entry = this->findInNeighourSet(neighbour, node->getNodeID());
	if (n_entry)
	{
		std::string log_msg = "Erasing DUPLICATE in neighbour set IP : " + n_entry->getIp() + " Port : " +
							  n_entry->getPort() + " NodeID: " + n_entry->getNodeID();
		LogHandler::getInstance().logMsg(log_msg);
		neighbour.erase(n_entry);
	}
	neighbour.insert(node);
	std::string log_msg = "adding to neighbour set IP : " + node->getIp() + " Port : " +
						  node->getPort() + " NodeID: " + node->getNodeID();
	LogHandler::getInstance().logMsg(log_msg);
	if (neighbour.size() > this->col)
	{
		std::string log_msg = "Erasing in OVERFLOW in neighbour set IP : " + (*neighbour.begin())->getIp() + " Port : " +
							  (*neighbour.begin())->getPort() + " NodeID: " + (*neighbour.begin())->getNodeID();
		LogHandler::getInstance().logMsg(log_msg);
		neighbour.erase(neighbour.begin());
	}
	return;
}

void ClientDatabase::addToRoutingTable(node_Sptr node, int prefix)
{
	// cout << "Entering in addToRoutingTable" << endl;
	if (!node)
	{
		return;
	}
	seeder_mtx.lock();
	if (this->is_same_node_as_me(node))
	{
		return;
	}
	seeder_mtx.unlock();

	int proximity = calculateProximity(node->getIp());
	node->setProximity(proximity);

	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	prefix = prefixMatchLen(this->listener->getNodeID(), node->getNodeID());
	auto index = node->getNodeID()[prefix] - '0';
	if (!this->routingTable[prefix][index])
	{
		this->routingTable[prefix][index] = node;
		std::string log_msg = "adding to routing table IP : " + node->getIp() + " Port : " +
							  node->getPort() + " NodeID: " + node->getNodeID() + " Prefix,index = " + to_string(prefix) +
							  +"," + to_string(index);
		LogHandler::getInstance().logMsg(log_msg);
		return;
	}
	if (node->getProximity() < this->routingTable[prefix][index]->getProximity())
	{
		this->routingTable[prefix][index] = node;
		std::string log_msg = "adding to routing table with Proximity IP : " + node->getIp() + " Port : " +
							  node->getPort() + " NodeID: " + node->getNodeID() + " Prefix,index = " + to_string(prefix) +
							  +"," + to_string(index);
		LogHandler::getInstance().logMsg(log_msg);
	}
	return;
}
void ClientDatabase::updateAllState(node_Sptr node)
{
	this->addToLeafSet(node);
	// cout<<"After add to leaf in update"<<endl;
	this->addToNeighhbourSet(node);
	// cout<<"After add to neigbour in update"<<endl;
	this->addToRoutingTable(node);
	// cout<<"After add to routing in update"<<endl;
	return;
}

void ClientDatabase::updateRoutingTable(vector<node_Sptr> row_entry, int index)
{
	for (auto node : row_entry)
	{
		this->addToRoutingTable(node);
	}
	return;
}

int ClientDatabase::getRowSize()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->row;
}

int ClientDatabase::getColSize()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->col;
}

void ClientDatabase::setTotalRouteLength(int s)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->total_route_length = s;
}

int ClientDatabase::getTotalRouteLength()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->total_route_length;
}
void ClientDatabase::incrementRecievedUpdateCount(int n)
{
	LogHandler::getInstance().logMsg("In incrementRecievedUpdateCount");
	LogHandler::getInstance().logMsg("before increment: " + std::to_string(this->recieved_update_count));
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->recieved_update_count += n;
	LogHandler::getInstance().logMsg("after increment: " + std::to_string(this->recieved_update_count));
}
int ClientDatabase::getRecievedUpdateCount()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->recieved_update_count;
}
void ClientDatabase::resetUpdateValues()
{
	LogHandler::getInstance().logMsg("Resetting route length parameters");
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->total_route_length = this->row;
	this->recieved_update_count = 0;
}

void ClientDatabase::insertIntoHashMap(std ::string key, std ::string value)
{
	LogHandler::getInstance().logMsg("Inserting into HASH Table key: " + key + " Value: " + value);
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->hashMap[key] = value;
	return;
}

unordered_map<string, string> ClientDatabase::getHashMap()
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	return this->hashMap;
}
string ClientDatabase::getHashMapValue(string key)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (this->hashMap.find(key) != this->hashMap.end())
		return this->hashMap[key];
	else
		throw ErrorMsg("Key not present");
}

void ClientDatabase::deleteFromHashMap(pair<string, string> entry_to_delete)
{
	LogHandler::getInstance().logMsg("deleteFromHashMap, deleting key : " + entry_to_delete.first);
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	this->hashMap.erase(entry_to_delete.first);
}

void ClientDatabase::deleteFromLeafSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (node)
	{
		auto &left_leaf = this->leafSet.first;
		auto &right_leaf = this->leafSet.second;
		auto leaf_entry = this->findInLeafSet(left_leaf, node->getNodeID());
		if (leaf_entry)
		{
			std::string log_msg = "deleteFrom Left Leaf Set IP : " + leaf_entry->getIp() +
								  "Port : " + leaf_entry->getPort() + " NodeID: " + leaf_entry->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
			left_leaf.erase(leaf_entry);
		}
		leaf_entry = this->findInLeafSet(right_leaf, node->getNodeID());
		if (leaf_entry)
		{
			std::string log_msg = "deleteFrom Right Leaf Set IP : " + leaf_entry->getIp() +
								  "Port : " + leaf_entry->getPort() + " NodeID: " + leaf_entry->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
			right_leaf.erase(leaf_entry);
		}
	}
} // delete this node from Neighbour set
void ClientDatabase::deleteFromNeighhbourSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	if (node)
	{
		auto &n_set = this->neighbourSet;
		auto n_entry = this->findInNeighourSet(n_set, node->getNodeID());
		if (n_entry)
		{
			std::string log_msg = "deleteFromNeighhbourSet IP : " + n_entry->getIp() +
								  "Port : " + n_entry->getPort() + " NodeID: " + n_entry->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
			n_set.erase(n_entry);
		}
		this->neighbourSet.erase(node);
	}
} // delete this node from leaf set
void ClientDatabase::deleteFromRoutingTable(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto prefix = prefixMatchLen(this->listener->getNodeID(), node->getNodeID());
	auto index = node->getNodeID()[prefix] - '0';
	this->routingTable[prefix][index] = NULL;
	std::string log_msg = "Deleting from routing table IP : " + node->getIp() + " Port : " +
						  node->getPort() + " NodeID: " + node->getNodeID() + " Prefix,index = " + to_string(prefix) +
						  +"," + to_string(index);
	LogHandler::getInstance().logMsg(log_msg);
	return;
}
bool ClientDatabase::is_same_node_as_me(node_Sptr node)
{
	return node->getNodeID() == this->listener->getNodeID();
}

void ClientDatabase::delete_from_all(node_Sptr node)
{
	this->deleteFromLeafSet(node);
	this->deleteFromNeighhbourSet(node);
	this->deleteFromRoutingTable(node);
}

node_Sptr ClientDatabase::findInLeafSet(set<node_Sptr, leafComparator> &lset, std::string nodeId)
{
	for (auto node : lset)
	{
		if (node->getNodeID() == nodeId)
		{
			return node;
		}
	}
	return NULL;
}
node_Sptr ClientDatabase::findInNeighourSet(std::set<node_Sptr, neighbourComparator> &nset, std::string nodeId)
{
	for (auto node : nset)
	{
		if (node->getNodeID() == nodeId)
		{
			return node;
		}
	}
	return NULL;
}

int ClientDatabase::findInLeafSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &left_leafSet = this->leafSet.first;
	auto result = this->findInLeafSet(left_leafSet, node->getNodeID());
	if (result)
	{
		return 0;
	}
	auto &right_leafSet = this->leafSet.second;
	result = this->findInLeafSet(right_leafSet, node->getNodeID());
	if (result)
	{
		return 1;
	}
	return -1;
}

node_Sptr ClientDatabase::findInNeighourSet(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto &neighbourSet = this->neighbourSet;
	auto result = this->findInNeighourSet(neighbourSet, node->getNodeID());
	return result;
}

pair<int, int> ClientDatabase::findInRoutingTable(node_Sptr node)
{
	std::lock_guard<std::mutex> lock(this->seeder_mtx);
	auto prefix = prefixMatchLen(this->listener->getNodeID(), node->getNodeID());
	auto index = node->getNodeID()[prefix] - '0';
	if (!this->routingTable[prefix][index])
	{
		prefix = index = -1;
	}
	return make_pair(prefix, index);
}
void ClientDatabase::lazyUpdateLeafSet(bool leaf_set_side)
{
	seeder_mtx.lock();
	auto left_iterator = this->leafSet.first.begin();
	auto right_iterator = this->leafSet.second.rbegin();
	seeder_mtx.unlock();
	node_Sptr fetch_from_node;
	bool update = false;
	while (true)
	{
		seeder_mtx.lock();
		if (!leaf_set_side)
		{
			auto temp_iterator = this->leafSet.first.end();
			if (left_iterator != temp_iterator)
			{
				fetch_from_node = *left_iterator;
			}
			else
			{
				seeder_mtx.unlock();
				break;
			}
		}
		else
		{
			auto temp_iterator = this->leafSet.second.rend();
			if (right_iterator != temp_iterator)
			{
				fetch_from_node = *right_iterator;
			}
			else
			{
				seeder_mtx.unlock();
				break;
			}
		}
		seeder_mtx.unlock();
		try
		{
			message::Message req_msg;
			req_msg.set_type("RequestLeafSet");
			PeerCommunicator peercommunicator(*fetch_from_node);
			peercommunicator.sendMsg(req_msg);
			auto recieved_msg = peercommunicator.recieveMsg();
			auto req = recieved_msg.responseleafset();
			auto myNodeID = this->listener->getNodeID();
			for (int i = 0; i < req.leaf().node_size(); i++)
			{
				auto nodeFrmMsg = req.leaf().node(i);
				node_Sptr new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());

				if (!leaf_set_side)
				{
					if (new_node->getNodeID() < myNodeID)
					{
						try
						{
							PeerCommunicator peercommunicator(*new_node);
							this->addToLeafSet(new_node);
							update = true;
							// break;
						}
						catch (ErrorMsg e)
						{
						}
						// break;
					}
				}
				else
				{
					if (new_node->getNodeID() > myNodeID)
					{
						try
						{

							PeerCommunicator peercommunicator(*new_node);
							this->addToLeafSet(new_node);
							update = true;
						}
						catch (ErrorMsg e)
						{
						}
					}
				}
			}
			if (!update)
			{
				throw ErrorMsg("Try next");
			}
			break;
		}
		catch (ErrorMsg e)
		{
			seeder_mtx.lock();
			if (!leaf_set_side)
			{
				advance(left_iterator, 1);
				if (left_iterator == this->leafSet.first.end())
				{
					seeder_mtx.unlock();
					break;
				}
			}
			else
			{
				++right_iterator;
				if (right_iterator == this->leafSet.second.rend())
				{
					seeder_mtx.unlock();
					break;
				}
			}
			seeder_mtx.unlock();
		}
	}
	return;
}
void ClientDatabase::lazyUpdateNeighbourSet()
{
	seeder_mtx.lock();
	if (this->neighbourSet.empty())
	{
		seeder_mtx.unlock();
		return;
	}
	auto neighbour_iterator = this->neighbourSet.begin();
	seeder_mtx.unlock();
	node_Sptr fetch_from_node;
	bool update = false;
	while (true)
	{
		seeder_mtx.lock();
		fetch_from_node = *neighbour_iterator;
		seeder_mtx.unlock();
		try
		{
			message::Message req_msg;
			req_msg.set_type("RequestNeighbourSet");
			PeerCommunicator peercommunicator(*fetch_from_node);
			peercommunicator.sendMsg(req_msg);
			auto recieved_msg = peercommunicator.recieveMsg();
			auto req = recieved_msg.responseneighbourset();
			auto myNodeID = this->listener->getNodeID();
			for (int i = 0; i < req.neighbours().node_size(); i++)
			{
				auto nodeFrmMsg = req.neighbours().node(i);
				node_Sptr new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());

				try
				{
					PeerCommunicator peercommunicator(*new_node);
					this->addToNeighhbourSet(new_node);
					update = true;
				}
				catch (ErrorMsg e)
				{
				}
			}
			if (update)
			{
				break;
			}
			else
			{
				throw ErrorMsg("Try Next");
			}
		}
		catch (ErrorMsg e)
		{
			seeder_mtx.lock();
			advance(neighbour_iterator, 1);
			if (neighbour_iterator == this->neighbourSet.end())
			{
				seeder_mtx.unlock();
				break;
			}
			seeder_mtx.unlock();
		}
	}
	return;
}
void ClientDatabase::lazyUpdateRoutingTable(pair<int, int> position)
{
	int prefix = position.first;
	int index = position.second;
	auto routingTable = this->getRoutingTable();
	bool update = true;
	for (int add = 0;; add++)
	{
		if (prefix + add == this->getRowSize())
		{
			return;
		}
		for (int i = 0; i < this->getColSize(); i++)
		{
			if (i != index and routingTable[prefix + add][i])
			{
				try
				{
					message::Message req_msg;
					req_msg.set_type("RequestRoutingEntry");
					auto temp = req_msg.mutable_requestroutingentry();
					temp->set_index(prefix);
					PeerCommunicator peercommunicator(*routingTable[prefix + add][i]);
					peercommunicator.sendMsg(req_msg);
					auto recieved_msg = peercommunicator.recieveMsg();
					auto req = recieved_msg.responseroutingentry();
					auto nodeFrmMsg = req.routingentry().node(index);
					node_Sptr new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
					if (new_node->getNodeID() == "-1")
					{
						continue;
					}
					try
					{
						PeerCommunicator peercommunicator(*new_node);
						this->addToRoutingTable(new_node);
						return;
					}
					catch (ErrorMsg e)
					{
					}
				}
				catch (ErrorMsg e)
				{
					continue;
				}
			}
		}
	}
	return;
}

void ClientDatabase::lockShutdown()
{
	this->shutdown_mtx.lock();
}
void ClientDatabase::unlockShutdown()
{
	this->shutdown_mtx.unlock();
}

void ClientDatabase::lockPrint()
{
	this->print_mtx.lock();
}
void ClientDatabase::unlockPrint()
{
	this->print_mtx.unlock();
}