#include "peerMessageHandler.h"
#include "message.pb.h"
#include "clientDatabase.h"
#include "networkInterfacer.h"
#include "utils.h"
#include <syslog.h>
#include <string>
#include <vector>
#include "peerCommunicator.h"
#include "errorMsg.h"
#include "logHandler.h"
using namespace std;

void PeerMessageHandler::handleJoinMeRequest(message::Message msg)
{
	std::string log_msg = "Handling JoinMe request for IP: " + msg.joinmemsg().ip() + " Port: " +
						  msg.joinmemsg().port() + " NodeID: " + msg.joinmemsg().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.joinmemsg();

	//updating the joining node with relevant data
	auto relevant_pairs = getRelevantKeyValuePairs(req.nodeid());
	if (!relevant_pairs.empty())
	{
		message::Message msg;
		msg.set_type("AddToHashTable");
		auto sender = msg.mutable_sender();
		populateMsgSender(sender, ClientDatabase::getInstance().getListener());
		auto *temp = msg.mutable_addtohashtable();
		auto hash_map_message = temp->mutable_hashmap();
		for (auto entry : relevant_pairs)
		{
			(*hash_map_message)[entry.first] = entry.second;
		}
		std::string log_msg = "Sending AddToHashTable request to IP: " + req.ip() + " Port: " + req.port();
		LogHandler::getInstance().logMsg(log_msg);
		PeerCommunicator peercommunicator(req.ip(), req.port());
		peercommunicator.sendMsg(msg);
	}

	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	//populating sender
	auto sender = routingUpdate.mutable_sender();
	populateMsgSender(sender, ClientDatabase::getInstance().getListener());
	auto temp = routingUpdate.mutable_routingupdate();
	temp->set_buddy(true);

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

	///Checking for lazy routing updates
	while (next_node_sptr->getNodeID() != ClientDatabase::getInstance().getListener()->getNodeID())
	{
		// cout << "in handle joinme request next not equal to current" << endl;
		message::Message req_msg;
		req_msg.set_type("Join");
		auto sender = req_msg.mutable_sender();
		populateMsgSender(sender, ClientDatabase::getInstance().getListener());
		auto join_msg = req_msg.mutable_joinmsg();
		join_msg->set_ip(req.ip());
		join_msg->set_port(req.port());
		join_msg->set_nodeid(req.nodeid());
		join_msg->set_row_index(prefix_match_len+1);
		try
		{
			std::string log_msg = "Forwarding Join request to IP: " + next_node_sptr->getIp() + 
												" Port: " + next_node_sptr ->getPort();
			LogHandler::getInstance().logMsg(log_msg);
			PeerCommunicator peercommunicator(*next_node_sptr);
			peercommunicator.sendMsg(req_msg);
			break;
		}
		catch (ErrorMsg e)
		{
			std::string log_msg = "FAILED Forwarding Join request to IP: " + next_node_sptr->getIp() + 
												" Port: " + next_node_sptr ->getPort();
			LogHandler::getInstance().logError(log_msg);
			this->handleLazyUpdates(next_node_sptr);
			next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->set_terminal(true);
		temp->set_prefix_match(prefix_match_len+1);
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
	log_msg = "Sending RoutingUpdate packet to IP: " + req.ip() + 
										" Port: " + req.port();
	LogHandler::getInstance().logMsg(log_msg);
	PeerCommunicator peercommunicator(Node(req.ip(), req.port(), req.nodeid()));
	peercommunicator.sendMsg(routingUpdate);
	return;
}




void PeerMessageHandler::handleJoinRequest(message::Message msg)
{
	std::string log_msg = "Handling Join request for IP: " + msg.joinmsg().ip() + " Port: " +
		   msg.joinmsg().port() + " NodeID: " + msg.joinmsg().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.joinmsg();

	//updating the joining node with relevant data
	auto relevant_pairs = getRelevantKeyValuePairs(req.nodeid());
	if (!relevant_pairs.empty())
	{
		message::Message msg;
		msg.set_type("AddToHashTable");
		auto sender = msg.mutable_sender();
		populateMsgSender(sender, ClientDatabase::getInstance().getListener());
		auto *temp = msg.mutable_addtohashtable();
		auto hash_map_message = temp->mutable_hashmap();
		for (auto entry : relevant_pairs)
		{
			(*hash_map_message)[entry.first] = entry.second;
		}
		std::string log_msg = "Sending AddToHashTable request to IP: " + req.ip() + " Port: " + req.port();
		LogHandler::getInstance().logMsg(log_msg);
		PeerCommunicator peercommunicator(req.ip(), req.port());
		peercommunicator.sendMsg(msg);
	}

	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->set_buddy(true);

	auto new_routingList = temp->mutable_routingentires();

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	auto prefix_match_len = prefixMatchLen(req.nodeid(), ClientDatabase::getInstance().getListener()->getNodeID());
	// auto temp_list = temp->add_routingentires();
	// temp_list->set_index(prefix_match_len);
	// auto routing_row = temp_list->mutable_nodelist();
	LogHandler::getInstance().logMsg("Recieved row index: " + to_string(req.row_index()));
	temp->set_prefix_match(max(prefix_match_len+1, req.row_index()));
	for (int i = req.row_index(); i <= prefix_match_len; i++)
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

	///Checking for lazy routing updates
	while (next_node_sptr->getNodeID() != ClientDatabase::getInstance().getListener()->getNodeID())
	{
		message::Message req_msg;
		req_msg.set_type("Join");
		auto sender = req_msg.mutable_sender();
		populateMsgSender(sender, ClientDatabase::getInstance().getListener());
		auto join_msg = req_msg.mutable_joinmsg();
		join_msg->set_ip(req.ip());
		join_msg->set_port(req.port());
		join_msg->set_nodeid(req.nodeid());
		join_msg->set_row_index(prefix_match_len+1);
		try
		{
			std::string log_msg = "Forwarding Join request to IP: " + next_node_sptr->getIp() + 
												" Port: " + next_node_sptr ->getPort();
			LogHandler::getInstance().logMsg(log_msg);
			PeerCommunicator peercommunicator(*next_node_sptr);
			peercommunicator.sendMsg(req_msg);
			break;
		}
		catch (ErrorMsg e)
		{
			std::string log_msg = "FAILED Forwarding Join request to IP: " + next_node_sptr->getIp() + 
												" Port: " + next_node_sptr ->getPort();
			LogHandler::getInstance().logError(log_msg);
			this->handleLazyUpdates(next_node_sptr);
			next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
		}
	}

	//adding leaf node
	if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
	{
		auto new_leaf_set = temp->mutable_leaf();
		temp->set_terminal(true);
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
	auto sender = routingUpdate.mutable_sender();
	populateMsgSender(sender, next_node_sptr);
	log_msg = "Sending RoutingUpdate packet to IP: " + req.ip() + 
										" Port: " + req.port();
	LogHandler::getInstance().logMsg(log_msg);
	PeerCommunicator peercommunicator(Node(req.ip(), req.port(), req.nodeid()));
	peercommunicator.sendMsg(routingUpdate);
	return;
}

void PeerMessageHandler::handleRoutingUpdateRequest(message::Message msg)
{
	std::string log_msg = "Handling Routing Update request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.routingupdate();
	auto sender = msg.sender();
	auto sender_node = make_shared<Node>(sender.ip(), sender.port(), sender.nodeid());
	ClientDatabase::getInstance().updateAllState(sender_node);
	auto routingList = req.routingentires();
	// cout << "in handle routing update, routing entries count " << req.routingentires_size() << endl;
	ClientDatabase::getInstance().incrementRecievedUpdateCount(req.routingentires_size());

	// cout << "in handle routing updatemake c";
	printNode(sender_node);

	for (int i = 0; i < req.routingentires_size(); i++)
	{
		auto temp = req.routingentires(i);
		vector<node_Sptr> tempNodeList;
		for (int j = 0; j < temp.nodelist().node_size(); j++)
		{
			auto nodeFrmMsg = temp.nodelist().node(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() == "-1")
			{
				new_node = NULL; //make_shared<Node>(nullptr);
			}
			else
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
			}
			tempNodeList.push_back(new_node);
			//update proximity
			ClientDatabase::getInstance().updateRoutingTable(tempNodeList, temp.index());
		}
	}
	if (req.buddy())
	{
		std::string log_msg = "In Handle Routing Update request Buddy TRUE";
		LogHandler::getInstance().logMsg(log_msg);
		for (int j = 0; j < req.neighbours().node_size(); j++)
		{
			auto nodeFrmMsg = req.neighbours().node(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
				//calculate proximity
				ClientDatabase::getInstance().addToLeafSet(new_node);
			}
		}
	}
	if (req.terminal())
	{
		std::string log_msg = "In Handle Routing Update request Terminal TRUE";
		LogHandler::getInstance().logMsg(log_msg);
		auto sender = msg.sender();
		auto sender_node = make_shared<Node>(sender.ip(), sender.port(), sender.nodeid());
		ClientDatabase::getInstance().setTotalRouteLength(req.prefix_match());
		ClientDatabase::getInstance().addToLeafSet(sender_node);
		for (int j = 0; j < req.leaf().node_size(); j++)
		{
			auto nodeFrmMsg = req.leaf().node(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() != "-1")
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
				ClientDatabase::getInstance().addToLeafSet(new_node);
			}
		}
	}
	LogHandler::getInstance().logMsg("Current route update count: " + std::to_string(ClientDatabase::getInstance().getRecievedUpdateCount())
				+  " required route update count: " + 
				std::to_string(ClientDatabase::getInstance().getTotalRouteLength()));
	if (ClientDatabase::getInstance().getRecievedUpdateCount() == ClientDatabase::getInstance().getTotalRouteLength())
	{
		this->sendAllStateUpdate();
	}
}

void PeerMessageHandler::sendAllStateUpdate()
{
	std::string log_msg = "In send ALL state update";
	LogHandler::getInstance().logMsg(log_msg);
	message::Message all_state_req;
	populateMsgSender(all_state_req.mutable_sender(), ClientDatabase::getInstance().getListener());
	all_state_req.set_type("AllStateUpdate");
	auto sender = all_state_req.mutable_sender();
	populateMsgSender(sender, ClientDatabase::getInstance().getListener());
	auto temp = all_state_req.mutable_allstateupdate();

	auto new_neighbour_set = temp->mutable_neighbours();
	//adding neighbourSet
	auto neighbourSet = ClientDatabase::getInstance().getNeighbourSet();
	for (auto neighbour_node : neighbourSet)
	{
		LogHandler::getInstance().logMsg("ALL STATE: Adding neighbour node with node id: " + neighbour_node->getNodeID());
		auto nnode = new_neighbour_set->add_node();
		nnode->set_ip(neighbour_node->getIp());
		nnode->set_port(neighbour_node->getPort());
		nnode->set_nodeid(neighbour_node->getNodeID());
	}

	auto new_routingList = temp->mutable_routingtable();
	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	for (int i = 0; i < routingTable.size(); i++)
	{
		auto temp_list = temp->add_routingtable();
		for (auto node : routingTable[i])
		{
			auto list_node = temp_list->add_node();
			if (node)
			{
				LogHandler::getInstance().logMsg("ALL STATE: Adding routing node with node id: " + node->getNodeID());
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
	auto new_leaf_set = temp->mutable_leaf();
	auto leafSet = ClientDatabase::getInstance().getLeafSet();
	for (auto leaf_node : leafSet.first)
	{
		LogHandler::getInstance().logMsg("ALL STATE: Adding left leaf node with node id: " + leaf_node->getNodeID());
		auto lnode = new_leaf_set->add_node();
		lnode->set_ip(leaf_node->getIp());
		lnode->set_port(leaf_node->getPort());
		lnode->set_nodeid(leaf_node->getNodeID());
	}
	for (auto leaf_node : leafSet.second)
	{
		LogHandler::getInstance().logMsg("ALL STATE: Adding right leaf node with node id: " + leaf_node->getNodeID());
		auto lnode = new_leaf_set->add_node();
		lnode->set_ip(leaf_node->getIp());
		lnode->set_port(leaf_node->getPort());
		lnode->set_nodeid(leaf_node->getNodeID());
	}

	//broadcasting to all nodes in all tables
	for (auto node : neighbourSet)
	{
		std::string log_msg = "Sending all state update request to neighbour IP: " + node->getIp() + " Port: " + node->getPort() + " NodeID: " + node->getNodeID();
		LogHandler::getInstance().logMsg(log_msg);
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(all_state_req);
	}
	for (auto node : leafSet.first)
	{
		std::string log_msg = "Sending all state update request to left leaf IP: " + node->getIp() + " Port: " + node->getPort() + " NodeID: " + node->getNodeID();
		LogHandler::getInstance().logMsg(log_msg);
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(all_state_req);
	}
	for (auto node : leafSet.second)
	{
		std::string log_msg = "Sending all state update request to right leaf IP: " + node->getIp() + " Port: " + node->getPort() + " NodeID: " + node->getNodeID();
		LogHandler::getInstance().logMsg(log_msg);
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(all_state_req);
	}
	for (int i = 0; i < routingTable.size(); i++)
	{
		for (auto node : routingTable[i])
		{
			if (node)
			{
				std::string log_msg = "Sending all state update request to routing entry IP: " + node->getIp() + " Port: " + node->getPort() + " NodeID: " + node->getNodeID();
				LogHandler::getInstance().logMsg(log_msg);
				PeerCommunicator peercommunicator(*node);
				peercommunicator.sendMsg(all_state_req);
			}
		}
	}
}

void PeerMessageHandler::handleAllStateUpdateRequest(message::Message msg)
{
	std::string log_msg = "Handling ALL State Update request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.allstateupdate();
	auto sender = msg.sender();
	auto sender_node = make_shared<Node>(sender.ip(), sender.port(), sender.nodeid());
	// cout << "in handle all state update";
	printNode(sender_node);
	ClientDatabase::getInstance().updateAllState(sender_node);
	for (int i = 0; i < req.leaf().node_size(); i++)
	{
		auto leaf_entry = req.leaf().node(i);
		node_Sptr node_from_msg;
		if (leaf_entry.nodeid() == "-1")
		{
			node_from_msg = NULL; //make_shared<Node>(nullptr);
		}
		else
		{
			node_from_msg = make_shared<Node>(leaf_entry.ip(), leaf_entry.port(), leaf_entry.nodeid());
			std::string log_msg = "Leaf Entry-> Ip: " + node_from_msg->getIp() + " Port: " +
		   				node_from_msg->getPort() + " NodeID: " + node_from_msg->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
		}
		ClientDatabase::getInstance().updateAllState(node_from_msg);
	}
	for (int i = 0; i < req.neighbours().node_size(); i++)
	{
		auto neighbour = req.neighbours().node(i);
		node_Sptr node_from_msg;
		if (neighbour.nodeid() == "-1")
		{
			node_from_msg = NULL; //make_shared<Node>(nullptr);
		}
		else
		{
			node_from_msg = make_shared<Node>(neighbour.ip(), neighbour.port(), neighbour.nodeid());
			std::string log_msg = "Neighbour Entry-> Ip: " + node_from_msg->getIp() + " Port: " +
		   				node_from_msg->getPort() + " NodeID: " + node_from_msg->getNodeID();
			LogHandler::getInstance().logMsg(log_msg);
		}
		ClientDatabase::getInstance().updateAllState(node_from_msg);
	}
	for (int i = 0; i < req.routingtable_size(); i++)
	{
		auto row_of_routing_table = req.routingtable(i);
		for (int j = 0; j < row_of_routing_table.node_size(); j++)
		{
			auto nodeFrmMsg = row_of_routing_table.node(j);
			node_Sptr new_node;
			if (nodeFrmMsg.nodeid() == "-1")
			{
				new_node = NULL; //make_shared<Node>(nullptr);
			}
			else
			{
				new_node = make_shared<Node>(nodeFrmMsg.ip(), nodeFrmMsg.port(), nodeFrmMsg.nodeid());
				std::string log_msg = "Routing Entry-> Ip: " + new_node->getIp() + " Port: " +
							new_node->getPort() + " NodeID: " + new_node->getNodeID();
				LogHandler::getInstance().logMsg(log_msg);
			};
			ClientDatabase::getInstance().updateAllState(new_node);
		}
	}
}
void PeerMessageHandler::handleGetValRequest(message::Message msg)
{
	std::string log_msg = "Handling Get Val request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.getvalmsg();
	do
	{
		auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.key());

		if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
		{
			message::Message resp;
			resp.set_type("GetValResponse");
			auto temp = resp.mutable_getvalresponse();
			temp->set_key(req.key());
			temp->set_value(ClientDatabase::getInstance().getHashMapValue(req.key()));
			//set value from hash table
			std::string log_msg = "Sending Get Val response to IP: " + req.node().ip() + " Port: " +
				req.node().port() + " NodeID: " + req.node().nodeid();
			LogHandler::getInstance().logMsg(log_msg);
			PeerCommunicator peercommunicator(Node(req.node().ip(), req.node().port(), req.node().nodeid()));
			peercommunicator.sendMsg(resp);
			break;
		}
		else
		{
			try
			{
				std::string log_msg = "Forwarding Get Val request to IP: " + next_node_sptr->getIp() + " Port: " +
					next_node_sptr->getPort() + " NodeID: " + next_node_sptr->getNodeID();
				LogHandler::getInstance().logMsg(log_msg);
				PeerCommunicator peercommunicator(*next_node_sptr);
				peercommunicator.sendMsg(msg);
				break;
			}
			catch (ErrorMsg e)
			{
				std::string log_msg = "FAILED Forwarding Get Val request to IP: " + next_node_sptr->getIp() + " Port: " +
					next_node_sptr->getPort() + " NodeID: " + next_node_sptr->getNodeID();
				LogHandler::getInstance().logError(log_msg);
				this->handleLazyUpdates(next_node_sptr);
				// ClientDatabase::getInstance().delete_from_all(next_node_sptr);
			}
		}
	} while (true);
}

void PeerMessageHandler::handleGetValResponse(message::Message msg)
{
	std::string log_msg = "Handling Get Val response from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.getvalresponse();
	cout << "Key: " << req.key() << " Value: " << req.value() << endl;
}

void PeerMessageHandler::handleSetValRequest(message::Message msg)
{
	std::string log_msg = "Handling Set Val request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto req = msg.setvalmsg();
	do
	{
		auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.key());

		if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
		{
			ClientDatabase::getInstance().insertIntoHashMap(req.key(), req.val());
			std::string log_msg = "Inserting into HASH TABLE, key: " + req.key() + " Value: " + req.val();
			LogHandler::getInstance().logMsg(log_msg);
			break;
			//update local hash table
		}
		else
		{
			try
			{
				std::string log_msg = "Forwarding Set Val request to IP: " + next_node_sptr->getIp() + " Port: " +
								next_node_sptr->getPort() + " NodeID: " + next_node_sptr->getNodeID();
				LogHandler::getInstance().logMsg(log_msg);
				PeerCommunicator peercommunicator(*next_node_sptr);
				peercommunicator.sendMsg(msg);
				break;
			}
			catch (ErrorMsg e)
			{
				std::string log_msg = "FAILED Forwarding Get Val request to IP: " + next_node_sptr->getIp() + " Port: " +
								next_node_sptr->getPort() + " NodeID: " + next_node_sptr->getNodeID();
				LogHandler::getInstance().logError(log_msg);
				this->handleLazyUpdates(next_node_sptr);
				// ClientDatabase::getInstance().delete_from_all(next_node_sptr);
			}
		}
	} while (true);
}
unordered_map<string, string> PeerMessageHandler::getRelevantKeyValuePairs(string nodeID)
{
	string myNodeId = ClientDatabase::getInstance().getListener()->getNodeID();
	auto hash_table = ClientDatabase::getInstance().getHashMap();
	unordered_map<string, string> result;
	for (auto message : hash_table)
	{
		if (is_better_node_for_message(nodeID, myNodeId, message.first))
		{
			result.insert(message);
		}
	}
	for (auto entry : result)
	{
		ClientDatabase::getInstance().deleteFromHashMap(entry);
	}
	return result;
}

void PeerMessageHandler::handleAddToHashTableRequest(message::Message msg)
{
	std::string log_msg = "Handling AddToHashTable request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto hash_table = msg.addtohashtable().hashmap();
	for (auto pair : hash_table)
	{
		ClientDatabase::getInstance().insertIntoHashMap(pair.first, pair.second);
	}
}
void PeerMessageHandler::handleDeleteNodeRequest(message::Message msg)
{
	std::string log_msg = "Handling DeleteNode request from IP: " + msg.sender().ip() + " Port: " +
		   msg.sender().port() + " NodeID: " + msg.sender().nodeid();
	LogHandler::getInstance().logMsg(log_msg);
	auto node = msg.deletenode().node();
	auto node_to_delete = make_shared<Node>(node.ip(), node.port(), node.nodeid());
	ClientDatabase::getInstance().delete_from_all(node_to_delete);
}

void PeerMessageHandler::handleShutdownRequest()
{
	std::string log_msg = "Handling ShutDown request";
	LogHandler::getInstance().logMsg(log_msg);
	message::Message msg;
	msg.set_type("ShutDown");
	auto leaf_set = ClientDatabase::getInstance().getLeafSet();
	auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
	auto routing_table = ClientDatabase::getInstance().getRoutingTable();
	for (auto node : leaf_set.first)
	{
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(msg);
	}
	for (auto node : leaf_set.second)
	{
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(msg);
	}
	for (auto node : neighbour_set)
	{
		PeerCommunicator peercommunicator(*node);
		peercommunicator.sendMsg(msg);
	}
	for (auto row_entry : routing_table)
	{
		for (auto node : row_entry)
		{
			if (node)
			{
				PeerCommunicator peercommunicator(*node);
				peercommunicator.sendMsg(msg);
			}
		}
	}
	exit(0);
}

void PeerMessageHandler::handleLazyUpdates(node_Sptr node)
{
	std::string log_msg = "Handling Lazy Updates for IP: " + node->getIp() + " Port: " + 
								node->getPort() + " NodeID " + node->getNodeID();
	LogHandler::getInstance().logMsg(log_msg);	
	auto leaf_set_side = ClientDatabase::getInstance().findInLeafSet(node);
	if (leaf_set_side != -1)
	{
		ClientDatabase::getInstance().deleteFromLeafSet(node);
		std::thread T(&ClientDatabase::lazyUpdateLeafSet, std::ref(ClientDatabase::getInstance()), leaf_set_side);
		T.detach();
	}
	else
	{
		ClientDatabase::getInstance().deleteFromLeafSet(node);
	}

	if (ClientDatabase::getInstance().findInNeighourSet(node))
	{
		ClientDatabase::getInstance().deleteFromNeighhbourSet(node);
		std::thread T(&ClientDatabase::lazyUpdateNeighbourSet, std::ref(ClientDatabase::getInstance()));
		T.detach();
	}
	else
	{
		ClientDatabase::getInstance().deleteFromNeighhbourSet(node);
	}
	auto position_in_routing_table = ClientDatabase::getInstance().findInRoutingTable(node);
	if (position_in_routing_table.first != -1)
	{
		ClientDatabase::getInstance().deleteFromRoutingTable(node);
		std::thread T(&ClientDatabase::lazyUpdateRoutingTable, std::ref(ClientDatabase::getInstance()), position_in_routing_table);
		T.detach();
	}
	else
	{
		ClientDatabase::getInstance().deleteFromRoutingTable(node);
	}
	return;
}

void PeerMessageHandler::handleRequestLeafSet(int peer_fd)
{
	std::string log_msg = "Handling LEAF set Request ";
	LogHandler::getInstance().logMsg(log_msg);	
	PeerCommunicator peercommunicator(peer_fd);
	message:: Message respMsg;
	respMsg.set_type("ResponseLeafSet");
	auto temp = respMsg.mutable_responseleafset();
	auto new_leaf_set = temp->mutable_leaf();
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
	peercommunicator.sendMsg(respMsg);
}

void PeerMessageHandler::handleRequestNeighbourSet(int peer_fd)
{
	std::string log_msg = "Handling Neighbour set Request ";
	LogHandler::getInstance().logMsg(log_msg);	
	PeerCommunicator peercommunicator(peer_fd);
	message:: Message respMsg;
	respMsg.set_type("ResponseNeighbourSet");
	auto temp = respMsg.mutable_responseneighbourset();
	auto new_neighbour_set = temp->mutable_neighbours();
	auto neighbourSet = ClientDatabase::getInstance().getNeighbourSet();
	for (auto neighbour_node : neighbourSet)
	{
		auto nnode = new_neighbour_set->add_node();
		nnode->set_ip(neighbour_node->getIp());
		nnode->set_port(neighbour_node->getPort());
		nnode->set_nodeid(neighbour_node->getNodeID());
	}
	peercommunicator.sendMsg(respMsg);
}

void PeerMessageHandler::handleRequestRoutingEntry(int peer_fd, message::Message reqMsg)
{	
	auto req = reqMsg.requestroutingentry();
	auto index = req.index();
	std::string log_msg = "Handling Routing entry request for " + req.index();
	LogHandler::getInstance().logMsg(log_msg);
	PeerCommunicator peercommunicator(peer_fd);

	message:: Message respMsg;
	respMsg.set_type("ResponseRoutingEntry");
	auto temp = respMsg.mutable_responseroutingentry();
	auto new_routingTableList = temp->mutable_routingentry();
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();

	for (auto node : routingTable[index])
	{
		auto list_node = new_routingTableList->add_node();
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
	peercommunicator.sendMsg(respMsg);	
}
