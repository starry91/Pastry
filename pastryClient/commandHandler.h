#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <iostream>
#include <string>
#include "message.pb.h"
using message::Response;
class CommandHandler
{
  public:
	void handleCommand(std::string);
	void leafSetRepairer();
};

#endif
