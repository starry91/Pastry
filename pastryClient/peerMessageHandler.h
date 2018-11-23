//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef PEERMESSAGEHANDLER_H
#define PEERMESSAGEHANDLER_H

#include <iostream>
#include <string>
#include "message.pb.h"
#include <vector>
class PeerMessageHandler
{
  public:
    void handleJoinMeRequest(message::Message);
    void handleJoinRequest(message::Message);
    void handleRoutingUpdateRequest(message::Message);
    void handleAllStateUpdateRequest(message::Message);
    void handleGetValRequest(message::Message);
    void handleSetValRequest(message::Message);
    void sendAllStateUpdate();
    // ClientDatabase::getInstance()->getHashTable();
    std::vector<std::pair<std::string, std::string>> getRelevantKeyValuePairs(std::string nodeID);
};

#endif