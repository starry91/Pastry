#include "peerMessageHandler.h"
#include "message.pb.h"
#include "clientDatabase.h"
#include "networkInterfacer.h"
#include "utils.h"
#include <syslog.h>
#include <string>
#include <vector>

using namespace std;

void PeerMessageHandler::handleJoinMeRequest(message::Message msg)
{
	auto req = msg.joinmemsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->buddy = true;

	auto new_routingList = temp->mutable_routingentires();
	auto new_neighbour_set = temp->mutable_neighbours();

	//adding neighbourSet
	auto neighbourSet = ClientDatabase::getInstance().getNeighbourSet();
	for (auto neighbour_node : neighbourSet)
	{
		auto nnode = new_neighbour_set->add_node();
		nnode->set_ip(neighbour_node->getIp());
		nnode->set_port(neighbour_node->getPort());
		nnode->set_nodeid(neighbour_node->getNodeID());
	}

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	auto prefix_match_len = prefixMatchLen(req.nodeid(), ClientDatabase::getInstance().getListener()->getNodeID());
	for (int i = 0; i <= prefix_match_len; i++)
	{
		auto temp_list = temp->add_routingentires();
		temp_list->set_index(i);
		auto routing_row = temp_list->mutable_nodelist();
		for (auto node : routingTable[i])
		{
			auto list_node = routing_row->add_node();
			if (node)
			{
				list_node->set_ip(node->getIp());
				list_node->set_port(node->getPort());
				list_node->set_nodeid(node->getNodeID());
			}
			else
			{
				list_node->set_ip("-1");
				list_node->set_port("-1");
				list_node->set_nodeid("-1");
			}
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == req.nodeid())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->terminal = true;
		auto leafSet = ClientDatabase::getInstance().getLeafSet();
		for (auto leaf_node : leafSet.first)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
		for (auto leaf_node : leafSet.second)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
	}
	int sock_fd = createTCPClient(req.ip(), req.port());
	NetworkWriter writer(sock_fd);
	auto reply_string = routingUpdate.SerializeAsString();
	writer.writeToNetwork(vector<char>(reply_string.begin(), reply_string.end()));
	delete &writer; //closing connection after writing

	if (next_node_sptr->getNodeID() != req.nodeid())
	{
		int sock_fd = createTCPClient(next_node_sptr->getIp(), next_node_sptr->getPort());
		NetworkWriter writer(sock_fd);
		auto reply_string = routingUpdate.SerializeAsString();
		writer.writeToNetwork(vector<char>(reply_string.begin(), reply_string.end()));
		delete &writer; //closing connection after writing
	}
}
void PeerMessageHandler::handleJoinRequest(message::Message msg)
{
	auto req = msg.joinmsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->buddy = true;

	auto new_routingList = temp->mutable_routingentires();

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	auto prefix_match_len = prefixMatchLen(req.nodeid(), ClientDatabase::getInstance().getListener()->getNodeID());
	auto temp_list = temp->add_routingentires();
	temp_list->set_index(prefix_match_len);
	auto routing_row = temp_list->mutable_nodelist();
	for (auto node : routingTable[prefix_match_len])
	{
		auto list_node = routing_row->add_node();
		if (node)
		{
			list_node->set_ip(node->getIp());
			list_node->set_port(node->getPort());
			list_node->set_nodeid(node->getNodeID());
		}
		else
		{
			list_node->set_ip("-1");
			list_node->set_port("-1");
			list_node->set_nodeid("-1");
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == req.nodeid())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->terminal = true;
		auto leafSet = ClientDatabase::getInstance().getLeafSet();
		for (auto leaf_node : leafSet.first)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
		for (auto leaf_node : leafSet.second)
		{
			auto lnode = new_leaf_set->add_node();
			lnode->set_ip(leaf_node->getIp());
			lnode->set_port(leaf_node->getPort());
			lnode->set_nodeid(leaf_node->getNodeID());
		}
	}
	int sock_fd = createTCPClient(req.ip(), req.port());
	NetworkWriter writer(sock_fd);
	auto reply_string = routingUpdate.SerializeAsString();
	writer.writeToNetwork(vector<char>(reply_string.begin(), reply_string.end()));
	delete &writer; //closing connection after writing

	if (next_node_sptr->getNodeID() != req.nodeid())
	{
		int sock_fd = createTCPClient(next_node_sptr->getIp(), next_node_sptr->getPort());
		NetworkWriter writer(sock_fd);
		auto reply_string = routingUpdate.SerializeAsString();
		writer.writeToNetwork(vector<char>(reply_string.begin(), reply_string.end()));
		delete &writer; //closing connection after writing
	}
}
void PeerMessageHandler::handleRoutingUpdateRequest(message::Message msg)
{
	auto req = msg.routingupdate();
	auto routingList = req.routingentires();
	for (int i = 0; i < req.routingentires_size(); i++)
	{
		auto temp = req.routingentires().Get(i);
		vector<node_Sptr> tempNodeList;
		for (int j = 0; j < temp.nodelist().node_size(); j++)
		{
			auto nodeFrmMsg = temp.nodelist().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() == "-1")
			{
				new_node = make_shared<Node>(nullptr);
			}
			else
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			}
			tempNodeList.push_back(new_node);
			//update routing table
		}
	}
	if (req.buddy)
	{
		for (int j = 0; j < req.neighbours().node_size(); j++)
		{
			auto nodeFrmMsg = req.neighbours().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			}
			//calculate proximity
			//update neighbourhoodset
		}
	}
	if (req.terminal)
	{
		for (int j = 0; j < req.leaf().node_size(); j++)
		{
			auto nodeFrmMsg = req.leaf().node().Get(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			}
			//update leafset
		}
	}
}
void PeerMessageHandler::handleAllStateUpdateRequest(message::Message)
{
}
void PeerMessageHandler::handleGetValRequest(message::Message)
{
}
void PeerMessageHandler::handleSetValRequest(message::Message)
{
}