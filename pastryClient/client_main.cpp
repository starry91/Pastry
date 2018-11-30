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
#include "printer.h"
using std::cout;
using std::endl;
using namespace std;

int main(int argc, char *argv[])
{
    cout<<"Welcome to Pasty client"<<endl;
    cout<<"You can Enter following commands"<<endl;
    cout<<"port <IP> <Port>\tInitialise pastry"<<endl;
    cout<<"port <port> \t\tIt will take LANs IP"<<endl;
    cout<<"create\t\t\tIt will create a pastry listener on the ablove given commands"<<endl;
    cout<<"join <IP> <Port> \tGive the ip and port of the pastry node through which you want to join"<<endl;
    cout<<"put <Key> <Value> \tIt will store the value against the given key"<<endl;
    cout<<"get <Key\t\tIt will retrieve the value stored against the key"<<endl;
    cout<<"nset\t\t\tPrints the Neighbourhood Set"<<endl;
    cout<<"lset \t\t\tPrints the Leaf Set"<<endl;
    cout<<"routetable\t\tPrints Route Table"<<endl;
    cout<<"hashTable\t\tPrints the Hash Table"<<endl;
    cout<<"quit\t\t\tGracefully closes the pastry node"<<endl;
    cout<<"shutdown\t\tShut Down the whole pastry Network"<<endl;
    syslog(0, "----------------------------------");
    if (argc == 2)
    {
        //Setting log path
        LogHandler::getInstance().setLogPath(string(argv[1]));
    }

    while (true)
    {
        //reading input command
        Custom_Printer().printToConsole("Enter Command...");
        std::string command;
        std::getline(std::cin, command);
        std::thread t1(&CommandHandler::handleCommand, CommandHandler(), command);
        t1.detach();
    }
}