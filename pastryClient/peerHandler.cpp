#include "peerHandler.h"
#include "networkInterfacer.h"
#include "decoder.h"
#include "peerMessageHandler.h"
#include "message.h"
#include "encoder.h"
#include "logHandler.h"
#include "errorMsg.h"
#include "clientDatabase.h"
#include "unistd.h"

using std::cout;
using std::endl;

void PeerHandler::handleRpc(int client_fd)
{
    try
    {
       // cout << "in PeerHandler::handleRpc() with fd: " << client_fd << endl;
        while (true)
        {
            NetworkReader reader(client_fd);
            auto byte_data = reader.readFromNetwork();
           // std::cout << "PeerHandler::handleRpc() byte_data size: " << byte_data.size() << std::endl;
            Decoder decoder;
            Encoder encoder;
            auto rpcbytepair = decoder.decodeMsgType(byte_data);
            auto request = rpcbytepair.first;
            byte_data = rpcbytepair.second;
            // if (byte_data.size() == 0)
            // {
            // }
            //std::string request = msg->getType();
            PeerMessageHandler msgHandler;
            NetworkWriter writer(client_fd);
            if (request == "CHUNKINFOREQUEST")
            {
                //std::cout << "Recieved ChunkInfoRequest on" << ClientDatabase::getInstance().getHost().getPort() << std::endl;
                LogHandler::getInstance().logMsg("Recieved ChunkInfoRequest request");
                //std::cout << "PeerHandler::handleRpc() hello1" << std::endl;
                auto res = msgHandler.handleChunkInfoRequest(byte_data);
                writer.writeToNetwork(encoder.encode(std::string("CHUNKINFORESPONSE"), res.getBytes()));
            }
            else if (request == "SENDCHUNKREQUEST")
            {
                //std::cout << "Recieved Send Chunk request on" << ClientDatabase::getInstance().getHost().getPort() << std::endl;
                LogHandler::getInstance().logMsg("Recieved Send Chunk request");
                //std::cout << "PeerHandler::handleRpc() hello2" << std::endl;
                auto res = msgHandler.handlesendChunkRequest(byte_data);
                writer.writeToNetwork(encoder.encode(std::string("SENDCHUNKRESPONSE"), res.getBytes()));
            }
        }
    }
    catch (ErrorMsg e)
    {
        //std::cout << "PeerHandler::handleRpc() Exception received: " << e.getErrorMsg() << "For fd: " << client_fd << std::endl;
    }
    close(client_fd);   
}