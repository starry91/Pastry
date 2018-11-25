//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef PEERCOMMUNICATOR_H
#define PEERCOMMUNICATOR_H

#include <string>
#include <iostream>
#include "message.pb.h"
#include "node.h"
#include <vector>

using message::Response;
using message::Message;

class PeerCommunicator
{
  int peer_fd;

public:
  PeerCommunicator(Node peer);
  PeerCommunicator(std::string ip, std::string port);
  Response sendMsg(Message msg);
  void connectToTracker();
  virtual ~PeerCommunicator();
  Message recieveMsg();
};

#endif