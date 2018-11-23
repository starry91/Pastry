#include "node.h"
#include "utils.h"
Node::Node(std::string ip, std::string port)
{
    this->ip = ip;
    this->port = port;
    this->nodeID = getHash(ip);
}

Node::Node(std::string ip_port)
{
    int index = ip_port.find(":");
    this->ip = ip_port.substr(0, index);
    this->port = ip_port.substr(index + 1, ip_port.length() - index);
    this->nodeID = getHash(this->ip);
}

Node::Node(std::string ip, std::string port, std::string nodeID) {
    this->ip = ip;
    this->port = port;
    this->nodeID = nodeID;    
}

std::string Node::getIp()
{
    return this->ip;
}
std::string Node::getPort()
{
    return this->port;
}
std::string Node::getPort()
{
    return this->nodeID;
}
Node::Node()
{
}