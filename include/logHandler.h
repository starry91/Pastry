#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <string>

//Logger class

class LogHandler
{
  std::string log_path;
  LogHandler();

public:
  static LogHandler &getInstance();
  void logMsg(std::string);
  void logError(std::string);
  void setLogPath(std::string path);
};

#endif