#include "message.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <string.h>
#include "utils.h"

using std::cout;
using std::endl;

//------------------------------------------------------------Message: Share----------------------------------------------------------------------------

//Constructors
Share::Share(std::vector<char> b)
{
  {
    uint32_t name_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->file_name = std::string(&b[4], &b[4 + name_size]);
    b.erase(b.begin(), b.begin() + 4 + name_size);
  }
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

Share::Share()
{
}

//Getters
std::string Share::getFileName()
{
  return this->file_name;
}

std::string Share::getHash()
{
  return this->hash;
}
std::string Share::getIp()
{
  return this->ip;
}
std::string Share::getPort()
{
  return this->port;
}

std::string Share::getType()
{
  return std::string("share");
}

std::vector<char> Share::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->file_name.size();
  buf = uint32tonv(size);
  for (auto i : this->file_name)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->hash.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->hash)
    {
      buf.push_back(i);
    }
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//setters
void Share::setFileName(std::string file_name)
{
  this->file_name = file_name;
}

void Share::setHash(std::string hash)
{
  this->hash = hash;
}
void Share::setIp(std::string ip)
{
  this->ip = ip;
}
void Share::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: AddSeeder----------------------------------------------------------------------------

//Constructors
AddSeeder::AddSeeder(std::vector<char> b)
{
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

AddSeeder::AddSeeder()
{
}

//Getters
std::string AddSeeder::getHash()
{
  return this->hash;
}
std::string AddSeeder::getIp()
{
  return this->ip;
}
std::string AddSeeder::getPort()
{
  return this->port;
}

std::string AddSeeder::getType()
{
  return std::string("AddSeeder");
}

std::vector<char> AddSeeder::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void AddSeeder::setHash(std::string hash)
{
  this->hash = hash;
}
void AddSeeder::setIp(std::string ip)
{
  this->ip = ip;
}
void AddSeeder::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: RemoveSeeder----------------------------------------------------------------------------

//Constructors
RemoveSeeder::RemoveSeeder(std::vector<char> b)
{
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

RemoveSeeder::RemoveSeeder()
{
}

//Getters
std::string RemoveSeeder::getHash()
{
  return this->hash;
}
std::string RemoveSeeder::getIp()
{
  return this->ip;
}
std::string RemoveSeeder::getPort()
{
  return this->port;
}

std::string RemoveSeeder::getType()
{
  return std::string("RemoveSeeder");
}

std::vector<char> RemoveSeeder::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void RemoveSeeder::setHash(std::string hash)
{
  this->hash = hash;
}
void RemoveSeeder::setIp(std::string ip)
{
  this->ip = ip;
}
void RemoveSeeder::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: SeederInfoRequest----------------------------------------------------------------------------

//Constructors
SeederInfoRequest::SeederInfoRequest(std::vector<char> b)
{
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
}

SeederInfoRequest::SeederInfoRequest()
{
}

//Getters
std::string SeederInfoRequest::getHash()
{
  return this->hash;
}

std::string SeederInfoRequest::getType()
{
  return std::string("SeederInfoRequest");
}

std::vector<char> SeederInfoRequest::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void SeederInfoRequest::setHash(std::string hash)
{
  this->hash = hash;
}

//------------------------------------------------------------Message: SeederInfoResponse----------------------------------------------------------------------------

//Constructors
SeederInfoResponse::SeederInfoResponse(std::vector<char> b)
{
  //hash
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->hash = std::string(&b[4], &b[4 + size]);
  //cout << "SeederInfoResponse() size, hash: " << size << this->hash << endl;
  b.erase(b.begin(), b.begin() + 4 + size);

  //status
  size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->status = std::string(&b[4], &b[4 + size]);
  //cout << "SeederInfoResponse() size, status: " << size << this->status << endl;
  b.erase(b.begin(), b.begin() + 4 + size);

  //seeder vector
  auto count = nvtouint32(std::vector<char>(&b[0], &b[4]));
  b.erase(b.begin(), b.begin() + 4);
  //cout << "SeederInfoResponse() count: " << count << endl;

  //cout << "SeederInfoResponse() before for loop vec size: " << b.size() << endl;

  for (int i = 0; i < count; i++)
  {
    uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    auto ip = std::string(&b[4], &b[4 + size]);
    b.erase(b.begin(), b.begin() + 4 + size);

    size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    auto port = std::string(&b[4], &b[4 + size]);
    b.erase(b.begin(), b.begin() + 4 + size);

    this->seeder_list.push_back(Seeder(ip, port));
  }
}

SeederInfoResponse::SeederInfoResponse()
{
}

//Getters
std::string SeederInfoResponse::getType()
{
  return std::string("SeederInfoResponse");
}

std::vector<char> SeederInfoResponse::getBytes()
{
  //buffer
  std::vector<char> buf;

  //hash
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), this->hash.begin(), this->hash.end());

  {
    //status
    size = this->status.size();
    auto temp = uint32tonv(size);
    buf.insert(buf.end(), temp.begin(), temp.end());
    buf.insert(buf.end(), this->status.begin(), this->status.end());
  }

  {
    //vectors
    size = this->seeder_list.size();
    auto temp = uint32tonv(size);
    buf.insert(buf.end(), temp.begin(), temp.end());
  }

  for (auto seeder : seeder_list)
  {
    auto tmp = uint32tonv(seeder.getIp().size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    auto ip = seeder.getIp();
    buf.insert(buf.end(), ip.begin(), ip.end());

    tmp = uint32tonv(seeder.getPort().size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    auto port = seeder.getPort();
    buf.insert(buf.end(), port.begin(), port.end());
  }
  return buf;
}

std::vector<Seeder> SeederInfoResponse::getSeeders()
{
  return this->seeder_list;
}

std::string SeederInfoResponse::getHash()
{
  return this->hash;
}

std::string SeederInfoResponse::getStatus()
{
  return this->status;
}

//Setters
void SeederInfoResponse::setHash(std::string hash)
{
  this->hash = hash;
}

void SeederInfoResponse::addSeeder(Seeder seed)
{
  this->seeder_list.push_back(seed);
}

void SeederInfoResponse::setStatus(std::string status)
{
  this->status = status;
}

//------------------------------------------------------------Message: ChunkInfoRequest----------------------------------------------------------------------------

//Constructors
ChunkInfoRequest::ChunkInfoRequest(std::vector<char> b)
{
  uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->hash = std::string(&b[4], &b[4 + hash_size]);
  b.erase(b.begin(), b.begin() + 4 + hash_size);
}

ChunkInfoRequest::ChunkInfoRequest()
{
}

//Getters
std::string ChunkInfoRequest::getHash()
{
  return this->hash;
}

std::string ChunkInfoRequest::getType()
{
  return std::string("ChunkInfoRequest");
}

std::vector<char> ChunkInfoRequest::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void ChunkInfoRequest::setHash(std::string hash)
{
  this->hash = hash;
}

//------------------------------------------------------------Message: ChunkInfoResponse----------------------------------------------------------------------------

//Constructors
ChunkInfoResponse::ChunkInfoResponse(std::vector<char> b)
{
  //Hash
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->hash = std::string(&b[4], &b[4 + size]);
  b.erase(b.begin(), b.begin() + 4 + size);

  //status
  size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->status = std::string(&b[4], &b[4 + size]);
  b.erase(b.begin(), b.begin() + 4 + size);

  //chunk vector
  auto count = nvtouint32(std::vector<char>(&b[0], &b[4]));
  b.erase(b.begin(), b.begin() + 4);

  for (int i = 0; i < count; i++)
  {
    uint32_t chunk = nvtouint32(std::vector<char>(&b[0], &b[4]));
    b.erase(b.begin(), b.begin() + 4);

    this->chunk_map.push_back(chunk);
  }
}

ChunkInfoResponse::ChunkInfoResponse()
{
}

//getType
std::string ChunkInfoResponse::getType()
{
  return std::string("ChunkInfoResponse");
}

//getBytes
std::vector<char> ChunkInfoResponse::getBytes()
{
  //buffer
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), this->hash.begin(), this->hash.end());

  //status
  size = this->status.size();
  auto temp = uint32tonv(size);
  buf.insert(buf.end(), temp.begin(), temp.end());
  buf.insert(buf.end(), this->status.begin(), this->status.end());

  //chunks
  size = this->chunk_map.size();
  temp = uint32tonv(size);
  buf.insert(buf.end(), temp.begin(), temp.end());

  for (auto val : this->chunk_map)
  {
    auto tmp = uint32tonv(val);
    buf.insert(buf.end(), tmp.begin(), tmp.end());
  }

  return buf;
}

//Getters
std::vector<u_int32_t> ChunkInfoResponse::getChunkInfo()
{
  return this->chunk_map;
}

std::string ChunkInfoResponse::getHash()
{
  return this->hash;
}

std::string ChunkInfoResponse::getStatus()
{
  return this->status;
}

//Setters
void ChunkInfoResponse::setChunkInfo(std::vector<u_int32_t> chunks)
{
  this->chunk_map = chunks;
}
void ChunkInfoResponse::setHash(std::string hash)
{
  this->hash = hash;
}

void ChunkInfoResponse::setStatus(std::string status)
{
  this->status = status;
}

//------------------------------------------------------------Message: SendChunkRequest----------------------------------------------------------------------------

//Constructor
SendChunkRequest::SendChunkRequest(std::vector<char> b)
{
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->hash = std::string(&b[4], &b[4 + size]);
  b.erase(b.begin(), b.begin() + 4 + size);

  this->chunk_index = nvtouint32(std::vector<char>(&b[0], &b[4]));
}

SendChunkRequest::SendChunkRequest()
{
}

//Getters
u_int32_t SendChunkRequest::getChunkId()
{
  return this->chunk_index;
}

std::string SendChunkRequest::getHash()
{
  return this->hash;
}

std::string SendChunkRequest::getType()
{
  return std::string("SendChunkRequest");
}

//getBytes
std::vector<char> SendChunkRequest::getBytes()
{
  //buffer
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), this->hash.begin(), this->hash.end());

  auto temp = uint32tonv(this->chunk_index);
  buf.insert(buf.end(), temp.begin(), temp.end());

  return buf;
}

//Setters
void SendChunkRequest::setChunkId(u_int32_t index)
{
  this->chunk_index = index;
}

void SendChunkRequest::setHash(std::string hash)
{
  this->hash = hash;
}

//------------------------------------------------------------Message: SendChunkResponse----------------------------------------------------------------------------

//Constructor
SendChunkResponse::SendChunkResponse(std::vector<char> &b)
{
  //Hash
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->hash = std::string(b.begin() + 4, b.begin() + 4 + size);
  b.erase(b.begin(), b.begin() + 4 + size);

  //status
  size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->status = std::string(&b[4], &b[4 + size]);
  b.erase(b.begin(), b.begin() + 4 + size);

  //Chunk Index
  this->chunk_index = nvtouint32(std::vector<char>(&b[0], &b[4]));
  b.erase(b.begin(), b.begin() + 4);

  //Data
  size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->chunk_data = std::vector<char>(b.begin() + 4, b.begin() + 4 + size);
}

SendChunkResponse::SendChunkResponse()
{
}

//Setters
void SendChunkResponse::setChunkData(std::vector<char> data)
{
  this->chunk_data = std::move(data);
}

void SendChunkResponse::setHash(std::string hash)
{
  this->hash = hash;
}

void SendChunkResponse::setChunkIndex(u_int32_t index)
{
  this->chunk_index = index;
}

void SendChunkResponse::setStatus(std::string status)
{
  this->status = status;
}

//Getters
std::string SendChunkResponse::getType()
{
  return std::string("SendChunkResponse");
}

std::vector<char> SendChunkResponse::getBytes()
{
  //buffer
  std::vector<char> buf;

  //hash
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), this->hash.begin(), this->hash.end());

  //status
  size = this->status.size();
  auto temp = uint32tonv(size);
  buf.insert(buf.end(), temp.begin(), temp.end());
  buf.insert(buf.end(), this->status.begin(), this->status.end());

  //chunkIndex
  temp = uint32tonv(this->chunk_index);
  buf.insert(buf.end(), temp.begin(), temp.end());

  //chunk data
  size = this->chunk_data.size();
  temp = uint32tonv(size);
  buf.insert(buf.end(), temp.begin(), temp.end());
  buf.insert(buf.end(), this->chunk_data.begin(), this->chunk_data.end());

  return buf;
}

std::vector<char> SendChunkResponse::getChunkdata()
{
  return this->chunk_data;
}

std::string SendChunkResponse::getHash()
{
  return this->hash;
}

u_int32_t SendChunkResponse::getChunkIndex()
{
  return this->chunk_index;
}

std::string SendChunkResponse::getStatus()
{
  return this->status;
}

//------------------------------------------------------------Message: Response(SUCCESS/FAIL)----------------------------------------------------------------------------
Response::Response(std::vector<char> &b)
{
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  this->response = std::string(&b[4], &b[4 + size]);
}

Response::Response()
{
}

std::string Response::getType()
{
  return std::string("Response");
}

std::vector<char> Response::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->response.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), response.begin(), response.end());
  return buf;
}

std::string Response::getResponse()
{
  return this->response;
}

void Response::setResponse(std::string response)
{
  this->response = response;
}

//------------------------------------------------------------Message: ----------------------------------------------------------------------------

//Constructors
SyncShare::SyncShare(std::vector<char> b)
{
  {
    uint32_t name_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->file_name = std::string(&b[4], &b[4 + name_size]);
    b.erase(b.begin(), b.begin() + 4 + name_size);
  }
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

SyncShare::SyncShare()
{
}

//Getters
std::string SyncShare::getFileName()
{
  return this->file_name;
}

std::string SyncShare::getHash()
{
  return this->hash;
}
std::string SyncShare::getIp()
{
  return this->ip;
}
std::string SyncShare::getPort()
{
  return this->port;
}

std::string SyncShare::getType()
{
  return std::string("SyncShare");
}

std::vector<char> SyncShare::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->file_name.size();
  buf = uint32tonv(size);
  for (auto i : this->file_name)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->hash.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->hash)
    {
      buf.push_back(i);
    }
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//setters
void SyncShare::setFileName(std::string file_name)
{
  this->file_name = file_name;
}

void SyncShare::setHash(std::string hash)
{
  this->hash = hash;
}
void SyncShare::setIp(std::string ip)
{
  this->ip = ip;
}
void SyncShare::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: SyncAddSeeder----------------------------------------------------------------------------

//Constructors
SyncAddSeeder::SyncAddSeeder(std::vector<char> b)
{
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

SyncAddSeeder::SyncAddSeeder()
{
}

//Getters
std::string SyncAddSeeder::getHash()
{
  return this->hash;
}
std::string SyncAddSeeder::getIp()
{
  return this->ip;
}
std::string SyncAddSeeder::getPort()
{
  return this->port;
}

std::string SyncAddSeeder::getType()
{
  return std::string("SyncAddSeeder");
}

std::vector<char> SyncAddSeeder::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void SyncAddSeeder::setHash(std::string hash)
{
  this->hash = hash;
}
void SyncAddSeeder::setIp(std::string ip)
{
  this->ip = ip;
}
void SyncAddSeeder::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: SyncRemoveSeeder----------------------------------------------------------------------------

//Constructors
SyncRemoveSeeder::SyncRemoveSeeder(std::vector<char> b)
{
  {
    uint32_t hash_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    this->hash = std::string(&b[4], &b[4 + hash_size]);
    b.erase(b.begin(), b.begin() + 4 + hash_size);
  }
  {
    uint32_t ip_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->ip = std::string(&b[4], &b[4 + ip_size]);
    b.erase(b.begin(), b.begin() + 4 + ip_size);
  }
  {
    uint32_t port_size = nvtouint32(std::vector<char>(&b[0], &b[4]));
    // std::cout << "hsize: " << hash_size << std::endl;
    this->port = std::string(&b[4], &b[4 + port_size]);
    b.erase(b.begin(), b.begin() + 4 + port_size);
  }
}

SyncRemoveSeeder::SyncRemoveSeeder()
{
}

//Getters
std::string SyncRemoveSeeder::getHash()
{
  return this->hash;
}
std::string SyncRemoveSeeder::getIp()
{
  return this->ip;
}
std::string SyncRemoveSeeder::getPort()
{
  return this->port;
}

std::string SyncRemoveSeeder::getType()
{
  return std::string("SyncRemoveSeeder");
}

std::vector<char> SyncRemoveSeeder::getBytes()
{
  std::vector<char> buf;
  uint32_t size = this->hash.size();
  buf = uint32tonv(size);
  for (auto i : this->hash)
  {
    buf.push_back(i);
  }

  {
    auto tmp = uint32tonv(this->ip.size());
    buf.insert(buf.end(), tmp.begin(), tmp.end());
    for (auto i : this->ip)
    {
      buf.push_back(i);
    }
  }

  auto tmp = uint32tonv(this->port.size());
  buf.insert(buf.end(), tmp.begin(), tmp.end());
  for (auto i : this->port)
  {
    buf.push_back(i);
  }

  return buf;
}

//Setters
void SyncRemoveSeeder::setHash(std::string hash)
{
  this->hash = hash;
}
void SyncRemoveSeeder::setIp(std::string ip)
{
  this->ip = ip;
}
void SyncRemoveSeeder::setPort(std::string port)
{
  this->port = port;
}

//------------------------------------------------------------Message: SyncSeederListRequest----------------------------------------------------------------------------

//Constructors
SyncSeederListRequest::SyncSeederListRequest()
{
}

//Getters

std::string SyncSeederListRequest::getType()
{
  return std::string("SyncSeederListRequest");
}

std::vector<char> SyncSeederListRequest::getBytes()
{
  return std::vector<char>();
}

//------------------------------------------------------------Message: SyncSeederListResponse----------------------------------------------------------------------------

//Constructors
SyncSeederListResponse::SyncSeederListResponse(std::vector<char> b)
{
  // std::cout << "In SyncSeederlist constructor, begenning" << std::endl;
  // for (auto i : b)
  // {
  //   std::cout << i;
  // }
  //hash
  uint32_t size = nvtouint32(std::vector<char>(&b[0], &b[4]));
  b.erase(b.begin(), b.begin() + 4);
  this->bytes = b;
  //std::cout << "In SyncSeederlist constructor, after" << std::endl;
}

SyncSeederListResponse::SyncSeederListResponse()
{
}

//Getters
std::string SyncSeederListResponse::getType()
{
  return std::string("SyncSeederListResponse");
}

std::vector<char> SyncSeederListResponse::getBytes()
{
  //buffer
  std::vector<char> buf;

  //bytes
  uint32_t size = this->bytes.size();
  buf = uint32tonv(size);
  buf.insert(buf.end(), this->bytes.begin(), this->bytes.end());
  return buf;
}

void SyncSeederListResponse::setBytes(std::vector<char> data)
{
  this->bytes = data;
}

std::vector<char> SyncSeederListResponse::getData()
{
  return this->bytes;
}