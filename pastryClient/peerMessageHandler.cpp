#include "peerMessageHandler.h"
#include "message.pb.h"
#include "clientDatabase.h"
#include <syslog.h>
#include <string>

using namespace std;

void PeerMessageHandler::handleJoinMeRequest(message::Message msg)
{
	auto req = msg.joinmemsg();
	auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(req.nodeid());
	message::Message routingUpdate;
	routingUpdate.set_type("RoutingUpdate");
	auto *temp = routingUpdate.mutable_routingupdate();
	temp->terminal = true;
	temp->buddy = true;

	auto new_routingList = temp->mutable_routingentires();
	auto new_leaf_set = temp->mutable_leaf();
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

	//adding leaf node
	if (next_node_sptr->getNodeID() == req.nodeid())
	{

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

	//adding routing entires
	auto routingTable = ClientDatabase::getInstance().getRoutingTable();
	for (auto neighbour_node : neighbourSet)
	{
		auto nnode = new_neighbour_set->add_node();
		nnode->set_ip(neighbour_node->getIp());
		nnode->set_port(neighbour_node->getPort());
		nnode->set_nodeid(neighbour_node->getNodeID());
	}

}
void PeerMessageHandler::handleJoinRequest(message::Message)
{
}
void PeerMessageHandler::handleRoutingUpdateRequest(message::Message)
{
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

// ChunkInfoResponse PeerMessageHandler::handleChunkInfoRequest(std::vector<char> b)
// {
//     ChunkInfoResponse res;
//     Decoder decoder;
//     std::shared_ptr<ChunkInfoRequest> m = std::make_shared<ChunkInfoRequest>(b);
//     try
//     {
//         auto &database = ClientDatabase::getInstance();
//         res.setHash(m->getHash());
//         res.setChunkInfo(database.getmTorrent(m->getHash())->getBitChunks());
//         res.setStatus("SUCCESS");
//         return res;
//     }
//     catch (...)
//     {
//         //std::cout << "fail in info" << std::endl;
//         ChunkInfoResponse res;
//         res.setHash(m->getHash());
//         res.setChunkInfo(std::vector<u_int32_t>(1, 1));
//         res.setStatus("FAIL");
//         return res;
//     }
// }

// SendChunkResponse PeerMessageHandler::handlesendChunkRequest(std::vector<char> b)
// {
//     SendChunkResponse res;
//     Decoder decoder;
//     std::shared_ptr<SendChunkRequest> m = std::make_shared<SendChunkRequest>(b);
//     try
//     {
//         auto &database = ClientDatabase::getInstance();
//         auto path = database.getmTorrent(m->getHash())->getPath();
//         std::vector<char> buf;
//         FileHandler fHandler;

//         fHandler.readFileChunk(m->getChunkId(), path, buf);
//         res.setHash(m->getHash());
//         res.setChunkIndex(m->getChunkId());
//         res.setChunkData(std::move(buf));
//         res.setStatus("SUCCESS");
//         return res;
//     }
//     catch (...)
//     {
//         // std::cout << "fail" << std::endl;
//         SendChunkResponse res;
//         res.setHash(m->getHash());
//         res.setChunkIndex(m->getChunkId());
//         res.setStatus("FAIL");
//         std::vector<char> b(1, 'a');
//         res.setChunkData(b);
//         return res;
//     }
// }