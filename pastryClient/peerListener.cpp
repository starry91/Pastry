#include "peerListener.h"
#include <arpa/inet.h>
#include <cstring>
#include "clientDatabase.h"
#include <thread>
#include "peerHandler.h"

using std::cout;
using std::endl;

void PeerListener::startListening()
{
    int server_fd, opt = 1;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Filling server information
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(ClientDatabase::getInstance().getListener()->getIp().c_str());;
    server_addr.sin_port = htons(stoi(ClientDatabase::getInstance().getListener()->getPort()));

    // Bind the socket with the server address
    if (bind(server_fd, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (true)
    {
        int cli_fd;
        if ((cli_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                             (socklen_t *)&client_addr)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        std::thread t1(&PeerHandler::handleRpc, PeerHandler(), cli_fd);
        t1.detach();
    }
}