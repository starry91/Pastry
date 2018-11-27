#include "printer.h"
#include "clientDatabase.h"
#include <string>
using namespace std;

void Custom_Printer::printToConsole(string msg)
{
    if (ClientDatabase::getInstance().getListener())
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "[" << ClientDatabase::getInstance().getListener()->getIp() << ":" << ClientDatabase::getInstance().getListener()->getPort() << "] ";
        cout << msg << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
    else
    {
        ClientDatabase::getInstance().lockPrint();
        cout << msg << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
}

void Custom_Printer::printNode(node_Sptr node)
{
    if (ClientDatabase::getInstance().getListener())
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "[" << ClientDatabase::getInstance().getListener()->getIp() << ":" << ClientDatabase::getInstance().getListener()->getPort() << "] ";
        cout << "NodeID: " << node->getNodeID() << " IP: " << node->getIp()
             << " Port: " << node->getPort() << " Proximity: " << node->getProximity() << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
    else
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "NodeID: " << node->getNodeID() << " IP: " << node->getIp()
             << " Port: " << node->getPort() << " Proximity: " << node->getProximity() << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
}

void Custom_Printer::printError(std::string msg)
{
    if (ClientDatabase::getInstance().getListener())
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "[" << ClientDatabase::getInstance().getListener()->getIp() << ":" << ClientDatabase::getInstance().getListener()->getPort() << "] ";
        cout << "\033[0;31m" << "ERROR: " << "\033[0m" << msg << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
    else
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "\033[0;31m" << "ERROR: " << "\033[0m" << msg << endl;
        ClientDatabase::getInstance().unlockPrint();
    }    
}