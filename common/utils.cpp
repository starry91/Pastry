#include "utils.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <istream>
#include <cstring>
#include <fstream>
#include <limits>
#include <math.h>
#include <syslog.h>
#include "errorMsg.h"
#include <openssl/md5.h>
// #include "md5.h"
#include <errno.h>
#include "proximity.h"
using std::cout;
using std::endl;
using namespace std;
uint32_t nvtouint32(const std::vector<char> &arr)
{
    uint32_t hsize;
    int i = 0;
    for (; i < sizeof(hsize); i++)
    {
        // std::cout << "char: " << (int)arr[i] << std::endl;
        ((char *)(&hsize))[i] = arr[i];
    }
    hsize = ntohl(hsize);
    return hsize;
}

std::vector<char> uint32tonv(uint32_t val)
{
    std::vector<char> buf;
    val = htonl(val);

    for (int i = 0; i < sizeof(val); i++)
    {
        // std::cout << "charbyte: " << (int)((char *)(&val))[i] << std::endl;
        buf.push_back(((char *)(&val))[i]);
    }
    return buf;
}

std::vector<char> readBytes(int n, int sock_fd)
{
    const uint32_t BUFSIZE = 1024 * 1024;
    char buf[BUFSIZE];
    int count = 0;
    std::vector<char> ebuf;
    // syslog(0, "Called readBytes fd: %d, n: %d", sock_fd, n);
    while (count < n)
    {
        // cout << "before read: count: " << count << ", n: " << n << endl;
        int temp_count = read(sock_fd, buf, BUFSIZE > (n - count) ? n - count : BUFSIZE);
        //cout << "after read: tmp_count: " << temp_count << endl;
        if (temp_count <= 0)
        {
            syslog(LOG_WARNING, "readBytes() connection closed, fd: %d, temp: %d, err: %s", sock_fd, temp_count, strerror(errno));
            throw ErrorMsg("connection  , fd: " + std::to_string(sock_fd));
        }
        for (int i = 0; i < temp_count; i++)
        {
            ebuf.push_back(buf[i]);
        }
        count += temp_count;
    }
    return ebuf;
}

std::vector<std::string> extractArgs(std::string command)
{
    char *pch;
    std::vector<std::string> args;
    pch = strtok((char *)command.c_str(), " ");
    while (pch != NULL)
    {
        args.push_back(std::string(pch));
        pch = strtok(NULL, " ");
    }
    return args;
}

int createTCPClient(string ip, string port)
{
    //cout << "In create TCP connection, Connecting to Tracker...." << endl;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw ErrorMsg("Socket creation error");
    }
    //cout << "In create TCP connection, socket created...." << endl;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    // syslog(0, "In create TCP connection, Host: %s, port: %s", client.getIp().c_str(), client.getPort().c_str());
    serv_addr.sin_port = htons(std::stoi(port));

    // Convert IPv4 and IPv6 addresses from text to binary form
    ///cout << "In create TCP connection, before itnet_pton...." << endl;
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        close(sock);
        throw ErrorMsg("Invalid address/ Address not supported");
    }
    //cout << "In create TCP connection, set ip...." << endl;
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        close(sock);
        throw ErrorMsg("Connection to " + ip + ":" + port + " failed");
    }
    //cout << "createTCPClient() Connected to Tracker with fd: " << sock << endl;
    return sock;
}

std::string getHash(std::string bytes, int len)
{
    unsigned char hash_buff[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)bytes.c_str(), bytes.size(), hash_buff);
    string nodeID = "";
    auto n = *(unsigned long long *)hash_buff;
    for (int i = 0; i < 64; i += len)
    {
        int res = 0;
        for (int j = 0; j < len; j++)
        {
            res *= 2;
            if (n & (1 << (i + j)))
                res++;
        }
        nodeID += char(res + '0');
    }
    return nodeID;
}

std::string trimString(std::string name, int len)
{
    return name.substr(0, len);
}

bool leafComparator::operator()(node_Sptr a, node_Sptr b)
{
    if (a->getNodeID().compare(b->getNodeID()) < 0)
        return true;
    else
        return false;
}

bool neighbourComparator::operator()(node_Sptr a, node_Sptr b)
{
    if (a->getProximity() == b->getProximity())
        return a->getNodeID() < b->getNodeID();
    if (a->getProximity() < b->getProximity())
        return true;
    else
        return false;
}

int prefixMatchLen(string x, string y)
{
    int i = 0;
    while (i < x.length() && x[i] == y[i])
        i++;
    return i;
}

bool is_better_node(node_Sptr node1, node_Sptr node2, string nodeID)
{
    if (!node1)
    {
        return false;
    }
    // auto node1_nodeID = node1->getNodeID();
    // auto node2_nodeID = node2->getNodeID();
    // int node1_dist = 0, node2_dist = 0, nodeID_dist = 0;
    // for (auto i = 0; i < node1_nodeID.length(); i++)
    // {
    //     node1_dist += node1_dist * 10 + (node1_nodeID[i] - '0');
    //     node2_dist += node2_dist * 10 + (node2_nodeID[i] - '0');
    //     nodeID_dist += nodeID_dist * 10 + (nodeID[i] - '0');
    //     if (node1_dist != node2_dist)
    //     {
    //         return abs(node1_dist - nodeID_dist) < abs(node2_dist - nodeID_dist);
    //     }
    // }
    // return false;
    auto new_node = node1->getNodeID();
    auto current_node = node2->getNodeID();
    bool lesser = false;
    bool greater = false;
    for (int i = 0; i < new_node.length(); i++)
    {
        if (new_node[i] != current_node[i])
        {
            if (lesser)
            {
                return new_node[i] < current_node[i];
            }
            else if (greater)
            {
                return new_node[i] > current_node[i];
            }
            else
            {
                return abs(new_node[i] - nodeID[i]) < abs(current_node[i] - nodeID[i]);
            }
        }
        if (new_node[i] > nodeID[i])
        {
            if (!greater)
            {
                lesser = true;
            }
        }
        else if (new_node[i] < nodeID[i])
        {
            if (!lesser)
            {
                greater = true;
            }
        }
    }
    return false;
}

bool is_better_node_for_message(string new_node, string current_node, string messageID)
{
    // int new_node_dist = 0, curr_node_dist = 0, msg_id__dist = 0;
    // for (auto i = 0; i < new_node.length(); i++)
    // {
    //     new_node_dist += new_node_dist * 10 + (new_node[i] - '0');
    //     curr_node_dist += curr_node_dist * 10 + (current_node[i] - '0');
    //     msg_id__dist += msg_id__dist * 10 + (messageID[i] - '0');
    //     if (new_node_dist != curr_node_dist)
    //     {
    //         return abs(new_node_dist - msg_id__dist) < abs(curr_node_dist - msg_id__dist);
    //     }
    // }
    // return false;
    bool lesser = false;
    bool greater = false;
    for (int i = 0; i < new_node.length(); i++)
    {
        if (new_node[i] != current_node[i])
        {
            if (lesser)
            {
                return new_node[i] < current_node[i];
            }
            else if (greater)
            {
                return new_node[i] > current_node[i];
            }
            else
            {
                return abs(new_node[i] - messageID[i]) < abs(current_node[i] - messageID[i]);
            }
        }
        if (new_node[i] > messageID[i])
        {
            if (!greater)
            {
                lesser = true;
            }
        }
        else if (new_node[i] < messageID[i])
        {
            if (!lesser)
            {
                greater = true;
            }
        }
    }
    return false;
}

void populateMsgSender(message::Node *sender, node_Sptr node)
{
    sender->set_ip(node->getIp());
    sender->set_port(node->getPort());
    sender->set_nodeid(node->getNodeID());
}

double calculateProximity(string ip_address) //rtt in msec
{
    char *ip_addr;
    ip_addr = (char *)ip_address.c_str();
    return proximity(ip_addr);
}