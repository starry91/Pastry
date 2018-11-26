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
        cout << msg << endl;
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
        cout << "NodeID: " << node->getNodeID() << " IP: " << node->getIp()
             << " Port: " << node->getPort() << " Proximity: " << node->getProximity() << endl;
    }
}

void Custom_Printer::printError(std::string msg)
{
    if (ClientDatabase::getInstance().getListener())
    {
        ClientDatabase::getInstance().lockPrint();
        cout << "[" << ClientDatabase::getInstance().getListener()->getIp() << ":" << ClientDatabase::getInstance().getListener()->getPort() << "] ";
        cout << "ERROR: " << msg << endl;
        ClientDatabase::getInstance().unlockPrint();
    }
    else
    {
        cout << "ERROR: " << msg << endl;
    }    
}