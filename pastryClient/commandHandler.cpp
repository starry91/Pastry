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
            LogHandler::getInstance().logError("In command handler -> join -> sending msg to ip " + temp->ip() + " port " + temp->port());
            peercommunicator.sendMsg(msg);
        }
        else if (args.size() == 3 && args[0] == "put")
        {
            string key = args[1];
            string value = args[2];
            key = getHash(key, config_parameter_b);
            // syslog(0,"hash value for %s is %s",args[1].c_str(),key.c_str());
            LogHandler::getInstance().logError("In command handler -> put -> hash value for " + arg[1] + " is " + key);
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
            peercommunicator.sendMsg(msg);
        }
        else if (args.size() == 2 && args[0] == "get")
        {
            string key = args[1];
            key = getHash(key, config_parameter_b);
            message::Message msg;
            msg.set_type("GetVal");
            auto sender = msg.mutable_sender();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
            auto *temp = msg.mutable_getvalmsg();
            temp->set_key(key);
            sender = temp->mutable_node();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
            auto nextNode = ClientDatabase::getInstance().getNextRoutingNode(key);
            if (nextNode->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
            {
                auto value = ClientDatabase::getInstance().getHashMapValue(key);
                printResponse(value); //printing get response to console
                return;
            }
            PeerCommunicator peercommunicator(*nextNode);
            peercommunicator.sendMsg(msg);
        }
        else if (args.size() == 1 && args[0] == "lset")
        {
            //printing leaf set
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
                    if (node)
                        printNode(node);
                }
            }
        }
        else if (args.size() == 1 && args[0] == "nset")
        {
            //print neighbours
            auto nSet = ClientDatabase::getInstance().getNeighbourSet();
            for (auto it : nSet)
            {
                printNode(it);
            }
        }
        else if (args.size() == 1 and args[0] == "quit")
        {
            LogHandler::getInstance().logError("Exiting");
            std ::cerr << "Exiting " << endl;
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
                    peercommunicator.sendMsg(delete_msg);
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
                    peercommunicator.sendMsg(delete_msg);
                }
            }
            auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
            for (auto node : neighbour_set)
            {
                PeerCommunicator peercommunicator(*node);
                peercommunicator.sendMsg(delete_msg);
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
                peercommunicator.sendMsg(msg);
            }
            exit(0);
        }
        else if (args.size() == 1 and args[0] == "shutdown")
        {
            LogHandler::getInstance().logError("Shutting Down");
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
        else if(args.size() == 1 and args[0] == "hashTable")
        {
            auto hashTable = ClientDatabase::getInstance().getHashMap();
            for(auto pair : hashTable)
            {
                cout << "Key: " << pair.first << " value: " << pair.second << endl;
            }
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

void CommandHandler::printResponse(std::string res)
{
    // LogHandler::getInstance().logMsg(res.status());
    cout << res << endl;
}

void CommandHandler::printError(std::string e)
{
    // LogHandler::getInstance().logMsg(e);
    cout << e << endl;
}