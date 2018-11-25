//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef PEERMESSAGEHANDLER_H
#define PEERMESSAGEHANDLER_H

#include <iostream>
#include <string>
#include "message.pb.h"
#include <vector>
#include "node.h"
class PeerMessageHandler
{
public:
  void handleJoinMeRequest(message::Message);
  void handleJoinRequest(message::Message);
  void handleRoutingUpdateRequest(message::Message);
  void handleAllStateUpdateRequest(message::Message);
  void handleGetValRequest(message::Message);
  void handleGetValResponse(message::Message);
  void handleSetValRequest(message::Message);
  void handleAddToHashTableRequest(message::Message);
  void handleDeleteNodeRequest(message::Message);
  void handleShutdownRequest();
  void sendAllStateUpdate();
  void handleLazyUpdates(node_Sptr node);
  std::unordered_map<std::string, std::string> getRelevantKeyValuePairs(std::string nodeID);
};

#endif