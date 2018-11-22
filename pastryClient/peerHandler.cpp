#include "peerHandler.h"
#include "networkInterfacer.h"
#include "peerMessageHandler.h"
#include "message.pb.h"
#include "logHandler.h"
#include "errorMsg.h"
#include "clientDatabase.h"
#include "unistd.h"

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
            reqMsg.ParseFromString(string(byte_data.data()));
            PeerMessageHandler msgHandler;
            NetworkWriter writer(client_fd);
            if (reqMsg.type() == "JoinMe")
            {
                LogHandler::getInstance().logMsg("Recieved JoinMe request");
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
                LogHandler::getInstance().logMsg("Recieved GetVal request");
                msgHandler.handleGetValRequest(reqMsg);
            }
            else if (reqMsg.type() == "SetVal")
            {
                LogHandler::getInstance().logMsg("Recieved SetVal request");
                msgHandler.handleSetValRequest(reqMsg);
            }
        }
    }
    catch (ErrorMsg e)
    {
        //std::cout << "PeerHandler::handleRpc() Exception received: " << e.getErrorMsg() << "For fd: " << client_fd << std::endl;
    }
    close(client_fd);   
}