#include <iostream>
#include <vector>
#include <string.h>
#include "seeder.h"
#include <thread>
#include <sstream>
#include <syslog.h>
#include <memory>
#include <vector>
#include <syslog.h>
#include <syslog.h>
#include "message.h"
#include "networkInterfacer.h"
#include "TrackerServiceServer.h"
#include "commandHandler.h"
#include "clientDatabase.h"
#include "logHandler.h"
#include "peerHandler.h"
#include "peerListener.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    if(argc == 2) {
        LogHandler::getInstance().setLogPath(string(argv[1]));
    }


    while (true)
    {
        std::cout << "Enter Command..." << std::endl;
        // cout << "in while in client main()" << endl;
        std::string command;
        std::getline(std::cin, command);
        // cout << "in while in client main() got command" << command <<endl;
        std::thread t1(&CommandHandler::handleCommand, CommandHandler(), command);
        t1.detach();
    }
}