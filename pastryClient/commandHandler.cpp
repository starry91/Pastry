#include "commandHandler.h"
#include "utils.h"
#include <syslog.h>
#include "clientDatabase.h"
#include "logHandler.h"
#include "errorMsg.h"
#include <map>
#include <queue>
#include <fstream>
#include <thread>
#include <chrono>
#include "utils.h"
#include <thread>
#include "message.pb.h"
#include <google/protobuf/message_lite.h>
#include "peerListener.h"
#include "networkInterfacer.h"
#include <vector>
#include <syslog.h>
#include "peerCommunicator.h"
using namespace std;

using std::cout;
using std::endl;

void CommandHandler::handleCommand(std::string command)
{
    try
    {
        std::vector<std::string> args = extractArgs(command);
        if (args.size() == 3 && args[0] == "port")
        {
            auto nodeID = getHash(args[1] + args[2], (config_parameter_b)); //b macro defined in Client Database
            auto trimmedNodeID = trimString(nodeID, ClientDatabase::getInstance().getRowSize());
            cout << trimmedNodeID << endl;
            ClientDatabase::getInstance().setListener(make_shared<Node>(Node(args[1], args[2], trimmedNodeID)));
        }
        else if (args.size() == 1 && args[0] == "create")
        {
            std::thread t1(&PeerListener::startListening, PeerListener());
            t1.detach();
        }
        else if (args.size() == 3 && args[0] == "join")
        {
            string ip = args[1];
            string port = args[2];
            message::Message msg;
            msg.set_type("JoinMe");
            auto *temp = msg.mutable_joinmemsg();
            temp->set_ip(ClientDatabase::getInstance().getListener()->getIp());
            temp->set_port(ClientDatabase::getInstance().getListener()->getPort());
            temp->set_nodeid(ClientDatabase::getInstance().getListener()->getNodeID());
            PeerCommunicator peercommunicator(ip, port);
            syslog(0, "In command handler -> join -> sending msg to ip %s port %s", temp->ip().c_str(), temp->port().c_str());
            auto resp = peercommunicator.sendMsg(msg);
            if (resp.status() == "FAIL")
            {
                syslog(0, "In command handler -> join -> recieved status: FAIL");
                LogHandler::getInstance().logError("JOINME - FAIL");
                throw ErrorMsg("Failed Join Me msg");
            }
            else
            {
                syslog(0, "In command handler -> join -> recieved status: SUCCESS");
                LogHandler::getInstance().logMsg("JOINME - SUCCESS");
            }
        }
        else if (args.size() == 3 && args[0] == "put")
        {
            string key = args[1];
            string value = args[2];
            key = getHash(key, config_parameter_b);
            message::Message msg;
            msg.set_type("SetVal");
            auto *temp = msg.mutable_setvalmsg();
            temp->set_key(key);
            temp->set_val(value);
            auto nextNode = ClientDatabase::getInstance().getNextRoutingNode(key);
            if (nextNode->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
            {
                ClientDatabase::getInstance().insertIntoHashMap(key, value);
                return;
            }
            PeerCommunicator peercommunicator(*nextNode);
            auto resp = peercommunicator.sendMsg(msg);
            if (resp.status() == "FAIL")
            {
                LogHandler::getInstance().logError("SET - FAIL");
                throw ErrorMsg("Failed Join Me msg");
            }
            else
            {
                LogHandler::getInstance().logMsg("SET - SUCCESS");
            }
        }
        else if (args.size() == 2 && args[0] == "get")
        {
            string key = args[1];
            key = getHash(key, config_parameter_b);
            message::Message msg;
            msg.set_type("GetVal");
            auto *temp = msg.mutable_getvalmsg();
            temp->set_key(key);
            auto nextNode = ClientDatabase::getInstance().getNextRoutingNode(key);
            if (nextNode->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
            {
                auto value = ClientDatabase::getInstance().getHashMapValue(key);
                ///Print the value on the screen
                return;
            }
            PeerCommunicator peercommunicator(*nextNode);
            auto resp = peercommunicator.sendMsg(msg);
            if (resp.status() == "FAIL")
            {
                LogHandler::getInstance().logError("GET - FAIL");
                throw ErrorMsg("Failed Join Me msg");
            }
            else
            {
                LogHandler::getInstance().logMsg("GET - SUCCESS");
            }
        }
        else if (args.size() == 1 && args[0] == "lset")
        {
            auto lSet = ClientDatabase::getInstance().getLeafSet();
            for (auto it : lSet.first)
            {
                printNode(it);
            }
            for (auto it : lSet.second)
            {
                printNode(it);
            }
        }
        else if (args.size() == 1 && args[0] == "routetable")
        {
            auto rTable = ClientDatabase::getInstance().getRoutingTable();
            //print the rTable
            for (auto it : rTable)
            {
                for (auto node : it)
                {
                    if (!node)
                        printNode(node);
                }
            }
        }
        else if (args.size() == 1 && args[0] == "nset")
        {
            auto nSet = ClientDatabase::getInstance().getNeighbourSet();
            for (auto it : nSet)
            {
                printNode(it);
            }
        }
        else if (args.size() == 1 and args[0] == "quit")
        {
            std ::cerr << "Exiting " << endl;
            // std::thread::id listener_thread_id = ClientDatabase::getInstance().getListenerThreadID();
            ////kill this listener thread
            node_Sptr best_leaf;
            auto leafSet = ClientDatabase::getInstance().getLeafSet();
            auto my_node = ClientDatabase::getInstance().getListener();
            auto my_nodeID = my_node->getNodeID();

            message::Message delete_msg;
            delete_msg.set_type("DeleteNode");
            auto *temp = delete_msg.mutable_deletenode();
            auto *node = temp->mutable_node();
            node->set_ip(my_node->getIp());
            node->set_nodeid(my_node->getNodeID());
            node->set_port(my_node->getPort());

            if (!leafSet.first.empty())
            {
                auto left_leafSet = leafSet.first;
                best_leaf = *left_leafSet.begin();
                for (auto leaf : left_leafSet)
                {
                    if (is_better_node(leaf, best_leaf, my_nodeID))
                    {
                        best_leaf = leaf;
                    }
                    PeerCommunicator peercommunicator(*leaf);
                    auto resp = peercommunicator.sendMsg(delete_msg);
                }
            }
            if (!leafSet.second.empty())
            {
                auto right_leafSet = leafSet.second;
                if (!best_leaf)
                {
                    best_leaf = *right_leafSet.begin();
                }
                for (auto leaf : right_leafSet)
                {
                    if (is_better_node(leaf, best_leaf, my_nodeID))
                    {
                        best_leaf = leaf;
                    }
                    PeerCommunicator peercommunicator(*leaf);
                    auto resp = peercommunicator.sendMsg(delete_msg);
                }
            }
            auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
            for (auto node : neighbour_set)
            {
                PeerCommunicator peercommunicator(*node);
                auto resp = peercommunicator.sendMsg(delete_msg);
            }
            if (best_leaf)
            {
                auto hash_table = ClientDatabase::getInstance().getHashMap();
                message::Message msg;
                msg.set_type("AddToHashTable");
                auto *temp = msg.mutable_addtohashtable();
                auto hash_map_message = temp->mutable_hashmap();
                for (auto entry : hash_table)
                {
                    (*hash_map_message)[entry.first] = entry.second;
                }
                PeerCommunicator peercommunicator(*best_leaf);
                auto resp = peercommunicator.sendMsg(msg);
            }
            exit(0);
        }
        else if (args.size() == 1 and args[0] == "shutdown")
        {
            message::Message msg;
            msg.set_type("ShutDown");
            auto leaf_set = ClientDatabase::getInstance().getLeafSet();
            auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
            auto routing_table = ClientDatabase::getInstance().getRoutingTable();
            for (auto node : leaf_set.first)
            {
                PeerCommunicator peercommunicator(*node);
                auto resp = peercommunicator.sendMsg(msg);
            }
            for (auto node : leaf_set.second)
            {
                PeerCommunicator peercommunicator(*node);
                auto resp = peercommunicator.sendMsg(msg);
            }
            for (auto node : neighbour_set)
            {
                PeerCommunicator peercommunicator(*node);
                auto resp = peercommunicator.sendMsg(msg);
            }
            for (auto row_entry : routing_table)
            {
                for (auto node : row_entry)
                {
                    if (node)
                    {
                        PeerCommunicator peercommunicator(*node);
                        auto resp = peercommunicator.sendMsg(msg);
                    }
                }
            }
            exit(0);
        }
        else
        {
            std::cerr << "Invalid Command" << endl;
        }
    }
    catch (ErrorMsg m)
    {
        LogHandler::getInstance().logError(m.getErrorMsg());
        this->printError(m.getErrorMsg());
    }
}

void CommandHandler::printResponse(std::string msg, message::Response res)
{
    // LogHandler::getInstance().logMsg("Request: " + msg);
    // LogHandler::getInstance().logMsg("Response: " + res.status());
    cout << res.status() << endl;
}

void CommandHandler::printResponse(Response res)
{
    // LogHandler::getInstance().logMsg(res.status());
    cout << res.status() << endl;
}

void CommandHandler::printError(std::string e)
{
    // LogHandler::getInstance().logMsg(e);
    cout << e << endl;
}