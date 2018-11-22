//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef NODE_H
#define NODE_H

#include <string>
#include <iostream>
#include <vector>
#include <memory>

class Node
{
    std::string ip;
    std::string port;
    std::string nodeID;
    int proximity;

  public:
    Node(std::string ip, std::string port);
    Node(std::string ip_port);
    Node();
    std::string getIp();
    std::string getPort();
    std::string getNodeID();
    int getProximity();
};

typedef std::shared_ptr<Node> node_Sptr;

#endif