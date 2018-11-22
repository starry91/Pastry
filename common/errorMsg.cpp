#include "errorMsg.h"

ErrorMsg::ErrorMsg(std::string msg)
{
    this->msg = msg;
}

std::string ErrorMsg::getErrorMsg()
{
    return this->msg;
}