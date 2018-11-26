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
#include "peerMessageHandler.h"
#include "printer.h"
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
            LogHandler::getInstance().logMsg("Node ID: " + trimmedNodeID);
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
            auto sender = msg.mutable_sender();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
            auto *temp = msg.mutable_joinmemsg();
            temp->set_ip(ClientDatabase::getInstance().getListener()->getIp());
            temp->set_port(ClientDatabase::getInstance().getListener()->getPort());
            temp->set_nodeid(ClientDatabase::getInstance().getListener()->getNodeID());
            PeerCommunicator peercommunicator(ip, port);
            LogHandler::getInstance().logMsg("In command handler -> join -> sending msg to ip " + ip + " port " + port);
            peercommunicator.sendMsg(msg);
            std ::thread T(&CommandHandler::leafSetRepairer, this);
            T.detach();
        }
        else if (args.size() == 3 && args[0] == "put")
        {
            string key = args[1];
            string value = args[2];
            key = getHash(key, (config_parameter_b));
            // syslog(0,"hash value for %s is %s",args[1].c_str(),key.c_str());
            LogHandler::getInstance().logMsg("In command handler -> put -> hash value for " + args[1] + " is " + key);

            message::Message msg;
            msg.set_type("SetVal");
            auto sender = msg.mutable_sender();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
            auto *temp = msg.mutable_setvalmsg();
            temp->set_key(key);
            temp->set_val(value);

            do
            {
                auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(key);

                if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
                {
                    ClientDatabase::getInstance().insertIntoHashMap(key, value);
                    std::string log_msg = "Inserting into HASH TABLE, key: " + key + " Value: " + value;
                    LogHandler::getInstance().logMsg(log_msg);
                    auto leaf_set = ClientDatabase::getInstance().getLeafSet();
                    msg.mutable_setvalmsg()->set_terminal(true);
                    //iterate for left leaf
                    for (auto it = leaf_set.first.rbegin(); it != leaf_set.first.rend(); it++)
                    {
                        try
                        {
                            std::string log_msg = "Forwarding left REPLICATE Set Val request to IP: " + (*it)->getIp() + " Port: " +
                                                  (*it)->getPort() + " NodeID: " + (*it)->getNodeID();
                            LogHandler::getInstance().logMsg(log_msg);
                            PeerCommunicator peercommunicator(**it);
                            peercommunicator.sendMsg(msg);
                            break;
                        }
                        catch (ErrorMsg e)
                        {
                            std::string log_msg = "FAILED Forwarding left REPLICATE Set Val request to IP: " + (*it)->getIp() + " Port: " +
                                                  (*it)->getPort() + " NodeID: " + (*it)->getNodeID();
                            LogHandler::getInstance().logMsg(log_msg);
                            PeerMessageHandler peerHandler;
                            peerHandler.handleLazyUpdates(*it);
                            continue;
                        }
                    }
                    //iterate for right leaf
                    for (auto it = leaf_set.second.begin(); it != leaf_set.second.end(); it++)
                    {
                        try
                        {
                            std::string log_msg = "Forwarding right REPLICATE Set Val request to IP: " + (*it)->getIp() + " Port: " +
                                                  (*it)->getPort() + " NodeID: " + (*it)->getNodeID();
                            LogHandler::getInstance().logMsg(log_msg);
                            PeerCommunicator peercommunicator(**it);
                            peercommunicator.sendMsg(msg);
                            break;
                        }
                        catch (ErrorMsg e)
                        {
                            std::string log_msg = "FAILED Forwarding right REPLICATE Set Val request to IP: " + (*it)->getIp() + " Port: " +
                                                  (*it)->getPort() + " NodeID: " + (*it)->getNodeID();
                            LogHandler::getInstance().logMsg(log_msg);
                            PeerMessageHandler peerHandler;
                            peerHandler.handleLazyUpdates(*it);
                            continue;
                        }
                    }
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
                        PeerMessageHandler peerHandler;
                        peerHandler.handleLazyUpdates(next_node_sptr);
                        // ClientDatabase::getInstance().delete_from_all(next_node_sptr);
                    }
                }
            } while (true);
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
            temp->set_actual_key(args[1]);
            sender = temp->mutable_node();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());

            do
            {
                auto next_node_sptr = ClientDatabase::getInstance().getNextRoutingNode(key);

                if (next_node_sptr->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
                {
                    auto value = ClientDatabase::getInstance().getHashMapValue(key);
                    // this->printResponse(value); //printing get response to console
                    string print_msg = "Key: " + temp->actual_key() + " Value: " + value;
                    Custom_Printer().printToConsole(print_msg);         
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
                        PeerMessageHandler peerHandler;
                        peerHandler.handleLazyUpdates(next_node_sptr);
                        // ClientDatabase::getInstance().delete_from_all(next_node_sptr);
                    }
                }
            } while (true);
        }
        else if (args.size() == 1 && args[0] == "lset")
        {
            //printing leaf set
            auto lSet = ClientDatabase::getInstance().getLeafSet();
            for (auto it : lSet.first)
            {
                Custom_Printer().printNode(it);
            }
            for (auto it : lSet.second)
            {
                Custom_Printer().printNode(it);
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
                        Custom_Printer().printNode(node);
                }
            }
        }
        else if (args.size() == 1 && args[0] == "nset")
        {
            //print neighbours
            auto nSet = ClientDatabase::getInstance().getNeighbourSet();
            for (auto it : nSet)
            {
                Custom_Printer().printNode(it);
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
            auto sender = delete_msg.mutable_sender();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
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
                    try
                    {
                        PeerCommunicator peercommunicator(*leaf);
                        peercommunicator.sendMsg(delete_msg);
                        if (is_better_node(leaf, best_leaf, my_nodeID))
                        {
                            best_leaf = leaf;
                        }
                    }
                    catch (ErrorMsg e)
                    {
                        continue;
                    }
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
                    try
                    {
                        PeerCommunicator peercommunicator(*leaf);
                        peercommunicator.sendMsg(delete_msg);
                        if (is_better_node(leaf, best_leaf, my_nodeID))
                        {
                            best_leaf = leaf;
                        }
                    }
                    catch (ErrorMsg e)
                    {
                        continue;
                    }
                }
            }
            auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
            for (auto node : neighbour_set)
            {
                try
                {
                    PeerCommunicator peercommunicator(*node);
                    peercommunicator.sendMsg(delete_msg);
                }
                catch (ErrorMsg e)
                {
                }
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
                try
                {
                    PeerCommunicator peercommunicator(*best_leaf);
                    peercommunicator.sendMsg(msg);
                }
                catch (ErrorMsg e)
                {
                }
            }
            exit(0);
        }
        else if (args.size() == 1 and args[0] == "shutdown")
        {
            ClientDatabase::getInstance().lockShutdown();
            LogHandler::getInstance().logError("Shutting Down");
            message::Message msg;
            msg.set_type("ShutDown");
            auto sender = msg.mutable_sender();
            populateMsgSender(sender, ClientDatabase::getInstance().getListener());
            auto leaf_set = ClientDatabase::getInstance().getLeafSet();
            auto neighbour_set = ClientDatabase::getInstance().getNeighbourSet();
            auto routing_table = ClientDatabase::getInstance().getRoutingTable();
            for (auto node : leaf_set.first)
            {
                try
                {
                    PeerCommunicator peercommunicator(*node);
                    peercommunicator.sendMsg(msg);
                }
                catch (ErrorMsg e)
                {
                }
            }
            for (auto node : leaf_set.second)
            {
                try
                {
                    PeerCommunicator peercommunicator(*node);
                    peercommunicator.sendMsg(msg);
                }
                catch (ErrorMsg e)
                {
                }
            }
            for (auto node : neighbour_set)
            {
                try
                {
                    PeerCommunicator peercommunicator(*node);
                    peercommunicator.sendMsg(msg);
                }
                catch (ErrorMsg e)
                {
                }
            }
            for (auto row_entry : routing_table)
            {
                for (auto node : row_entry)
                {
                    if (node)
                    {
                        try
                        {
                            PeerCommunicator peercommunicator(*node);
                            peercommunicator.sendMsg(msg);
                        }
                        catch (ErrorMsg e)
                        {
                        }
                    }
                }
            }
            ClientDatabase::getInstance().unlockShutdown();
            exit(0);
        }
        else if (args.size() == 1 and args[0] == "hashTable")
        {
            auto hashTable = ClientDatabase::getInstance().getHashMap();
            for (auto pair : hashTable)
            {
                std::string print_msg = "Value: " + pair.second;
                Custom_Printer().printToConsole(print_msg);
            }
        }
        else
        {
            std::string print_msg = "Invalid Command";
            Custom_Printer().printError(print_msg);
        }
    }
    catch (ErrorMsg m)
    {
        LogHandler::getInstance().logError(m.getErrorMsg());
        // this->printError(m.getErrorMsg());
        Custom_Printer().printError(m.getErrorMsg());
    }
}

void CommandHandler::leafSetRepairer()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(timer_limit));
        auto leaf_set = ClientDatabase::getInstance().getLeafSet();
        for (auto leaf : leaf_set.first)
        {
            try
            {
                PeerCommunicator peercommunicator(*leaf);
            }
            catch (ErrorMsg e)
            {
                string logMsg = "Removing Node in leafSetRepair Ip: " + leaf->getIp() + " Port: " +
                                leaf->getPort() + " NodeId: " + leaf->getNodeID();
                LogHandler::getInstance().logMsg(logMsg);
                PeerMessageHandler peerHandler;
                peerHandler.handleLazyUpdates(leaf);
            }
        }
        for (auto leaf : leaf_set.second)
        {
            try
            {
                PeerCommunicator peercommunicator(*leaf);
            }
            catch (ErrorMsg e)
            {
                string logMsg = "Removing Node in leafSetRepair Ip: " + leaf->getIp() + " Port: " +
                                leaf->getPort() + " NodeId: " + leaf->getNodeID();
                LogHandler::getInstance().logMsg(logMsg);
                PeerMessageHandler peerHandler;
                peerHandler.handleLazyUpdates(leaf);
            }
        }
    }
}
