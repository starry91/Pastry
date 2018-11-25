#ifndef PEERHANDLER_H
#define PEERHANDLER_H

#include <iostream>
#include <string>
#include <map>

class PeerHandler
{
public:
  void handleRpc(int sock_fd);
};

#endif