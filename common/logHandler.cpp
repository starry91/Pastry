#include "logHandler.h"
#include <fstream>
#include <ctime>
#include <iostream>

using std::cout;
using std::endl;

LogHandler::LogHandler(){};

LogHandler &LogHandler::getInstance()
{
    static LogHandler log;
    return log;
}

void LogHandler::logMsg(std::string msg)
{
    //cout << "LOgged: [" << msg << "]" << endl;
    //std::cout << "printing log to: " << this->log_path << std::endl;
    std::ofstream outfile;
    outfile.open(this->log_path, std::ios_base::app);
    std::time_t result = std::time(nullptr);
    outfile << std::asctime(std::localtime(&result)) << ": " << msg << std::endl;
    outfile.close();
}
void LogHandler::logError(std::string msg)
{
    //cout << "LOgged: [" << msg << "]" << endl;
    //std::cout << "printing log to: " << this->log_path << std::endl;
    std::ofstream outfile;
    outfile.open(this->log_path, std::ios_base::app);
    std::time_t result = std::time(nullptr);
    outfile << std::asctime(std::localtime(&result)) << ": Error - " << msg << std::endl;
    outfile.close();
}
void LogHandler::setLogPath(std::string path)
{
    this->log_path = path;
    std::ofstream outfile;
    outfile.open(this->log_path, std::ios_base::trunc);
    outfile.close();
}
