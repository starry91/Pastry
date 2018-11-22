//Name: Praveen Balireddy
//Roll: 2018201052

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include "utils.h"
#include "seeder.h"

//------------------------------------------------------------Base Class: Message----------------------------------------------------------------------------
class Message
{
public:
  virtual std::string getType() = 0;
  virtual std::vector<char> getBytes() = 0;
};

//------------------------------------------------------------Message: Share----------------------------------------------------------------------------
class Share : public Message
{
  std::string file_name;
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  std::string getFileName();
  Share(std::vector<char>);
  Share();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  void setFileName(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: AddSeeder----------------------------------------------------------------------------
class AddSeeder : public Message
{
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  AddSeeder(std::vector<char>);
  AddSeeder();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: RemoveSeeder----------------------------------------------------------------------------
class RemoveSeeder : public Message
{
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  RemoveSeeder(std::vector<char>);
  RemoveSeeder();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SeederInfoRequest----------------------------------------------------------------------------
class SeederInfoRequest : public Message
{
  std::string hash;

public:
  std::string getHash();
  SeederInfoRequest(std::vector<char>);
  SeederInfoRequest();
  void setHash(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SeederInfoResponse----------------------------------------------------------------------------
class SeederInfoResponse : public Message
{
  std::string hash;
  std::string status;
  std::vector<Seeder> seeder_list;

public:
  SeederInfoResponse(std::vector<char> b);
  SeederInfoResponse();
  void addSeeder(Seeder seeder);
  void setHash(std::string hash);
  void setStatus(std::string status);
  std::vector<Seeder> getSeeders();
  std::string getHash();
  std::string getStatus();
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: ChunkInfoRequest----------------------------------------------------------------------------
class ChunkInfoRequest : public Message
{
  std::string hash;

public:
  std::string getHash();
  ChunkInfoRequest(std::vector<char>);
  ChunkInfoRequest();
  void setHash(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: ChunkInfoResponse----------------------------------------------------------------------------
class ChunkInfoResponse : public Message
{
  std::string hash;
  std::string status;
  std::vector<u_int32_t> chunk_map;

public:
  std::vector<u_int32_t> getChunkInfo();
  std::string getHash();
  std::string getStatus();
  ChunkInfoResponse(std::vector<char> chunks);
  ChunkInfoResponse();
  void setChunkInfo(std::vector<u_int32_t> chunk_map);
  void setHash(std::string hash);
  void setStatus(std::string status);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SendChunkRequest----------------------------------------------------------------------------
class SendChunkRequest : public Message
{
  std::string hash;
  u_int32_t chunk_index;

public:
  u_int32_t getChunkId();
  std::string getHash();
  SendChunkRequest(std::vector<char>);
  SendChunkRequest();
  void setChunkId(u_int32_t chunk_index);
  void setHash(std::string hash);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SendChunkResponse----------------------------------------------------------------------------
class SendChunkResponse : public Message
{
  std::string hash;
  std::string status;
  u_int32_t chunk_index;
  std::vector<char> chunk_data;

public:
  std::vector<char> getChunkdata();
  std::string getHash();
  u_int32_t getChunkIndex();
  std::string getStatus();
  SendChunkResponse(std::vector<char> &b);
  SendChunkResponse();
  void setChunkData(std::vector<char> data);
  void setHash(std::string hash);
  void setChunkIndex(u_int32_t index);
  void setStatus(std::string status);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: Response(SUCCESS/FAIL)----------------------------------------------------------------------------
class Response : public Message
{
  std::string response;

public:
  std::string getResponse();
  Response(std::vector<char> &b);
  Response();
  void setResponse(std::string response);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SyncShare----------------------------------------------------------------------------
class SyncShare : public Message
{
  std::string file_name;
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  std::string getFileName();
  SyncShare(std::vector<char>);
  SyncShare();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  void setFileName(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SyncAddSeeder----------------------------------------------------------------------------
class SyncAddSeeder : public Message
{
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  SyncAddSeeder(std::vector<char>);
  SyncAddSeeder();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SyncRemoveSeeder----------------------------------------------------------------------------
class SyncRemoveSeeder : public Message
{
  std::string hash;
  std::string ip;
  std::string port;

public:
  std::string getHash();
  std::string getIp();
  std::string getPort();
  SyncRemoveSeeder(std::vector<char>);
  SyncRemoveSeeder();
  void setHash(std::string);
  void setIp(std::string);
  void setPort(std::string);
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SyncSeederList----------------------------------------------------------------------------
class SyncSeederListRequest : public Message
{
public:
  SyncSeederListRequest();
  virtual std::string getType();
  virtual std::vector<char> getBytes();
};

//------------------------------------------------------------Message: SeederInfoResponse----------------------------------------------------------------------------
class SyncSeederListResponse : public Message
{
  std::vector<char> bytes;

public:
  SyncSeederListResponse(std::vector<char> b);
  SyncSeederListResponse();
  virtual std::string getType();
  virtual std::vector<char> getBytes();
  std::vector<char> getData();
  void setBytes(std::vector<char> data);
  
};

#endif