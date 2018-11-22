//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef PEERMESSAGEHANDLER_H
#define PEERMESSAGEHANDLER_H

#include <iostream>
#include <string>
#include "message.h"
class PeerMessageHandler
{
  public:
    ChunkInfoResponse handleChunkInfoRequest(std::vector<char> b);
    SendChunkResponse handlesendChunkRequest(std::vector<char> b);
};

#endif