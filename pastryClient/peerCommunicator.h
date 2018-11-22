//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef TRACKERSERVICESERVER_H
#define TRACKERSERVICESERVER_H

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
  Response sendMsg(Message msg);
  void connectToTracker();
  virtual ~PeerCommunicator();
};

#endif