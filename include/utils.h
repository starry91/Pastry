
//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include "node.h"

uint32_t nvtouint32(const std::vector<char> &arr);
std::vector<char> uint32tonv(uint32_t val);
std::vector<char> readBytes(int n, int sock_fd);
std::vector<std::string> extractArgs(std::string command);
int createTCPClient(string ip, string port);
std::string getHash(std::string bytes);
struct leafComparator
{
    bool operator()(node_Sptr a, node_Sptr b);
};
struct neighbourComparator
{
    bool operator()(node_Sptr a, node_Sptr b);
};

int prefixMatchLen(string x, string y);
std ::string convertHexStringToNodeID(std::string s, int length);
#endif
