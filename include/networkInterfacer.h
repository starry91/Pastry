//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <iostream>
#include <vector>
#define magicStart "starry@iit0"
#define magicEnd "starry@iit1"

class NetworkReader
{
  int client_fd;

public:
  std::vector<char> readFromNetwork(int sock_fd);
  std::vector<char> readFromNetwork();
  NetworkReader(int sock_fd);
  NetworkReader();
};

class NetworkWriter
{
  int client_fd;

public:
  void writeToNetwork(int sock_fd, std::vector<char> b);
  void writeToNetwork(std::vector<char> b);
  NetworkWriter();
  NetworkWriter(int sock_fd);
};

#endif