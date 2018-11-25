#include "peerHandler.h"
#include "networkInterfacer.h"
#include "peerMessageHandler.h"
#include "message.pb.h"
#include "logHandler.h"
#include "errorMsg.h"
#include "clientDatabase.h"
#include "unistd.h"
#include <vector>
#include <syslog.h>
using std::cout;
using std::endl;
using namespace std;
void PeerHandler::handleRpc(int client_fd)
{
    try
    {
        while (true)
        {
            NetworkReader reader(client_fd);
            auto byte_data = reader.readFromNetwork();
            message::Message reqMsg;
            reqMsg.ParseFromArray(byte_data.data(), byte_data.size());
            PeerMessageHandler msgHandler;
            NetworkWriter writer(client_fd);
            if (reqMsg.type() == "JoinMe")
            {
                LogHandler::getInstance().logMsg("Recieved JoinMe request for IP: " + reqMsg.sender().ip() + " Port: " + reqMsg.sender().port());
                msgHandler.handleJoinMeRequest(reqMsg);
            }
            else if (reqMsg.type() == "Join")
            {
                LogHandler::getInstance().logMsg("Recieved Join request");
                msgHandler.handleJoinRequest(reqMsg);
            }
            else if (reqMsg.type() == "RoutingUpdate")
            {
                LogHandler::getInstance().logMsg("Recieved RoutingUpdate request");
                msgHandler.handleRoutingUpdateRequest(reqMsg);
            }
            else if (reqMsg.type() == "AllStateUpdate")
            {
                LogHandler::getInstance().logMsg("Recieved AllStateUpdate request");
                msgHandler.handleAllStateUpdateRequest(reqMsg);
            }
            else if (reqMsg.type() == "GetVal")
            {
                msgHandler.handleGetValRequest(reqMsg);
            }
            else if (reqMsg.type() == "GetValResponse")
            {
                LogHandler::getInstance().logMsg("Recieved GetValResponse request");
                msgHandler.handleGetValResponse(reqMsg);
            }
            else if (reqMsg.type() == "SetVal")
            {
                LogHandler::getInstance().logMsg("Recieved SetVal request");
                msgHandler.handleSetValRequest(reqMsg);
            }
            else if (reqMsg.type() == "DeleteNode")
            {
                LogHandler::getInstance().logMsg("Recieved DeleteNode request");
                msgHandler.handleDeleteNodeRequest(reqMsg);
            }
            else if (reqMsg.type() == "AddToHashTable")
            {
                LogHandler::getInstance().logMsg("Recieved AddToHashTable request");
                msgHandler.handleAddToHashTableRequest(reqMsg);
            }
            else if(reqMsg.type() == "ShutDown"){
                LogHandler::getInstance().logMsg("Recieved Shutdown request");
                msgHandler.handleShutdownRequest();
            }
        }
    }
    catch (ErrorMsg e)
    {
        LogHandler::getInstance().logError("PeerHandler::handleRpc() Exception received: " + e.getErrorMsg() + " For fd: " + std::to_string(client_fd));
    }
    close(client_fd);   
}