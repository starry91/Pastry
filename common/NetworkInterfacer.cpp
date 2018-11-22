#include "networkInterfacer.h"
#include <unistd.h>
#include "utils.h"
#include <cstdint>
#include <cassert>

using std::cout;
using std::endl;

//-------------------------------------------------------------Network Reader
NetworkReader::NetworkReader(int sock_fd)
{
    this->client_fd = sock_fd;
    //cout << "### Created NetworkReader with fd: " <<  this->client_fd << endl;
}

std::vector<char> NetworkReader::readFromNetwork()
{
    auto magic_start_b = readBytes(11, this->client_fd);

    if (std::string(magic_start_b.begin(), magic_start_b.end()) == magicStart)
    {
        auto header_b = readBytes(4, this->client_fd);
        uint32_t payload_size = nvtouint32(header_b);
        //std::cout << "readFromNetwork() Payload size: " << payload_size << std::endl;
        auto payload = readBytes(payload_size, this->client_fd);

        auto magic_start_e = readBytes(11, this->client_fd);
        //std::cout << "readFromNetwork() expected: " << std::string(magic_start_e.begin(), magic_start_e.end()) << " Actual: " << magicEnd << std::endl;
        if (std::string(magic_start_e.begin(), magic_start_e.end()) == magicEnd)
        {
            //std::cout << "readFromNetwork() Payload size debug: " << payload.size() << std::endl;
            return payload;
        }
        else
        {
            //std::cout << "readFromNetwork() Else in if" << std::endl;
            std::vector<char> v;
            return v;
        }
    }
    else
    {
        // std::cout << "readFromNetwork() Else in network" << std::endl;
        std::vector<char> v;
        return v;
    }
}

//---------------------------------------------------------------Network Writer

NetworkWriter::NetworkWriter(int sock_fd)
{
    this->client_fd = sock_fd;
}

void NetworkWriter::writeToNetwork(std::vector<char> b)
{
    //cout << "NetworkWriter::writeToNetwork() writing bytes: " << b.size() << endl;
    std::string magic_start = magicStart;
    write(this->client_fd, magic_start.c_str(), magic_start.length());
    auto payload_size = uint32tonv(b.size());
    //std::cout << "Payload size before sending: " << b.size() << " " << payload_size.size() << std::endl;
    write(this->client_fd, reinterpret_cast<char *>(payload_size.data()), payload_size.size());
    write(this->client_fd, reinterpret_cast<char *>(b.data()), b.size());
    std::string magic_end = magicEnd;
    write(this->client_fd, magic_end.c_str(), magic_end.length());
}
