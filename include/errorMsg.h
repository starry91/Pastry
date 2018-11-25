#ifndef ERROR_H
#define ERROR_H

#include <string>
//Error class to handle errors

class ErrorMsg
{
    std::string msg;

  public:
    ErrorMsg(std::string msg);
    std::string getErrorMsg();
};
#endif