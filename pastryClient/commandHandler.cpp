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
            key = getHash(key,config_parameter_b);
            message::Message msg;
            msg.set_type("GetVal");
            auto *temp = msg.mutable_getvalmsg();
            temp->set_key(key);
            auto nextNode = ClientDatabase::getInstance().getNextRoutingNode(key);
            if (nextNode->getNodeID() == ClientDatabase::getInstance().getListener()->getNodeID())
            {
                value = ClientDatabase::getInstance().getHashMapValue(key);
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
            auto rTable = ClientDatabase::getInstance().getLeafSet();
            //print the rTable
        }
        else if (args.size() == 1 && args[0] == "routetable")
        {
            auto rTable = ClientDatabase::getInstance().getRoutingTable();
            //print the rTable
        }
        else if (args.size() == 1 && args[0] == "nset")
        {
            auto rTable = ClientDatabase::getInstance().getNeighbourSet();
            //print the rTable
        }
        else if (args.size() == 1 and args[0] == "quit")
        {
            std ::cerr << "Exiting " << endl;
            auto hash_table = ClientDatabase::getInstance().getHash();
            // std::thread::id listener_thread_id = ClientDatabase::getInstance().getListenerThreadID();
            ////kill this listener thread 
            node_Sptr best_leaf;
            auto leafSet = ClientDatabase::getInstance().getLeafSet();
            auto my_nodeID = ClientDatabase::getInstance(.getListener()->getNodeID();

            message::Message delete_msg;
            delete_msg.set_type("DeleteNode");
            auto *temp = delete_msg.mutable_getvalmsg();
            auto *node = temp->mutable_getvalnode();
            temp->set_key(key);

            if(!leafSet.first.empty()){
                auto left_leafSet = leafSet.first;
                best_leaf = *left_leafSet.begin();
                for(leaf: left_leafSet){
                    if(is_better_node(leaf, best_leaf, my_nodeID)){
                        best_leaf = leaf;
                    }
                    PeerCommunicator peercommunicator(leaf);

                }
            }
            for(hash_entry : hash_table){
                
            }
            exit(0);
        }
        else if (args.size() == 1 and args[0] == "shutdown")
        {
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