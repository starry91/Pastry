#include <iostream>
#include <vector>
#include <string.h>
#include <thread>
#include <sstream>
#include <syslog.h>
#include <memory>
#include <vector>
#include <string>
#include "commandHandler.h"
#include "clientDatabase.h"
#include "logHandler.h"

using std::cout;
using std::endl;
using namespace std;

int main(int argc, char *argv[])
{
    syslog(0, "----------------------------------");
    if (argc == 2)
    {
        //Setting log path
        LogHandler::getInstance().setLogPath(string(argv[1]));
    }

    while (true)
    {
        //reading input command
        std::cout << "Enter Command..." << std::endl;
        std::string command;
        std::getline(std::cin, command);
        std::thread t1(&CommandHandler::handleCommand, CommandHandler(), command);
        t1.detach();
    }
}