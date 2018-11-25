
#include "networkInterfacer.h"
#include "utils.h"
#include <syslog.h>
#include <arpa/inet.h>
#include <string.h>
#include "utils.h"
#include "unistd.h"
#include "errorMsg.h"
#include "peerCommunicator.h"
#include <vector>
using namespace std;

PeerCommunicator::PeerCommunicator(Node peer)
{
    try
    {
        this->peer_fd = createTCPClient(peer.getIp(), peer.getPort());
    }
    catch (ErrorMsg e)
    {
        throw ErrorMsg("Unable to establish connection");
    }
    // cout << "### Created TrackerServiceServer with fd: " << this->tracker_fd << endl;
}

PeerCommunicator::PeerCommunicator(std::string ip, std::string port)
{
    try
    {
        this->peer_fd = createTCPClient(ip, port);
    }
    catch (ErrorMsg e)
    {
        throw ErrorMsg("Unable to establish connection");
    }
    // cout << "### Created TrackerServiceServer with fd: " << this->tracker_fd << endl;
}

PeerCommunicator::PeerCommunicator(int peer_fd)
{
    this->peer_fd = peer_fd;
}

PeerCommunicator::~PeerCommunicator()
{
    syslog(LOG_INFO, "### Closing PeerCommunicator() with fd: %d", this->peer_fd);
    close(this->peer_fd);
}

void PeerCommunicator::sendMsg(Message msg)
{
    NetworkWriter writer(this->peer_fd);
    size_t size = msg.ByteSizeLong();
    void *buffer = malloc(size);
    msg.SerializeToArray(buffer, size);
    writer.writeToNetwork(vector<char>((char *)buffer, (char *)buffer + size));
}

Message PeerCommunicator::recieveMsg()
{
    NetworkReader reader(this->peer_fd);
    auto byte_data = reader.readFromNetwork();
    message::Message reqMsg;
    reqMsg.ParseFromArray(byte_data.data(), byte_data.size());
    return reqMsg;
}