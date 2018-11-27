#include "proximity.h"
#include <errno.h>
using namespace std;

// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64

// Automatic port number
#define PORT_NO 0

// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1

// ping packet structure
struct ping_pkt
{
    struct icmphdr hdr;
    char msg[PING_PKT_S - sizeof(struct icmphdr)];
};

// Calculating the Check Sum
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    while (len > 1)
    {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1)
    {
        *(unsigned char *)&result = *(unsigned char *)buf;
        sum += result;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// make a ping request
double send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *ping_ip)
{
    int ttl_val = 64, msg_count = 0, i, addr_len, flag = 1,
        msg_received_count = 0;

    struct ping_pkt pckt;
    struct sockaddr_in r_addr;
    struct timespec time_start, time_end, tfs, tfe;
    double rtt_msec = 0, total_msec = 0, total_rtt = 0, avg_rtt;
    struct timeval tv_out;
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;

    clock_gettime(CLOCK_MONOTONIC, &tfs);

    // set socket options at ip to TTL and value to 64,
    // change to what you want by setting ttl_val
    if (setsockopt(ping_sockfd, SOL_IP, IP_TTL,
                   &ttl_val, sizeof(ttl_val)) != 0)
    {
        // printf("\nSetting socket options   to TTL failed!\n");
        return __DBL_MAX__;
    }

    else
    {
        // printf("\nSocket set to TTL..\n");
    }

    // setting timeout of recv setting
    setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO,
               (const char *)&tv_out, sizeof tv_out);

    // send icmp packet in an infinite loop

    // flag is whether packet was sent or not
    flag = 1;

    //filling packet
    bzero(&pckt, sizeof(pckt));

    pckt.hdr.type = ICMP_ECHO;
    pckt.hdr.un.echo.id = getpid();

    for (i = 0; i < sizeof(pckt.msg) - 1; i++)
        pckt.msg[i] = i + '0';

    pckt.msg[i] = 0;
    pckt.hdr.un.echo.sequence = msg_count++;
    pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));

    //usleep(PING_SLEEP_RATE);

    //send packet
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0,
               (struct sockaddr *)ping_addr,
               sizeof(*ping_addr)) <= 0)
    {
        // printf("\nPacket Sending Failed!\n");
        flag = 0;
    }

    //receive packet
    addr_len = sizeof(r_addr);

    if (recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,
                 (struct sockaddr *)&r_addr, (socklen_t *)&addr_len) <= 0 &&
        msg_count > 1)
    {
        // printf("\nPacket receive failed!\n");
    }

    else
    {
        clock_gettime(CLOCK_MONOTONIC, &time_end);

        double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
        rtt_msec = (time_end.tv_sec -
                    time_start.tv_sec) *
                       1000.0 +
                   timeElapsed;

        // if packet was not sent, don't receive
        if (flag)
        {
            if (!(pckt.hdr.type == 69 && pckt.hdr.code == 0))
            {
                // printf("Error..Packet received with ICMP type %d code %d\n", pckt.hdr.type, pckt.hdr.code);
            }
            else
            {
                msg_received_count++;
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &tfe);

    return rtt_msec;
}

// Driver Code
double proximity(char *ip_addr)
{
    int sockfd;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    char net_buf[NI_MAXHOST];
    double rtt = 0;
    // if(argc!=2)
    // {
    //     printf("\nFormat %s <address>\n", argv[0]);
    //     return 0;
    // }

    //ip_addr = dns_lookup(argv[1], &addr_con);
    struct hostent *host_entity;
    if ((host_entity = gethostbyname(ip_addr)) == NULL)
    {
        // No ip found for hostname
        //
    }

    //filling up address structure
    strcpy(ip_addr, inet_ntoa(*(struct in_addr *)
                                   host_entity->h_addr));

    (addr_con).sin_family = host_entity->h_addrtype;
    (addr_con).sin_port = htons(PORT_NO);
    (addr_con).sin_addr.s_addr = *(long *)host_entity->h_addr;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("\nSocket file descriptor not received: %s\n", strerror(errno));
        return 0;
    }
    // else
        // printf("\nSocket file descriptor %d received\n", sockfd);

    //send pings continuously
    rtt = send_ping(sockfd, &addr_con, ip_addr);
    return rtt;
}

// int main()
// {
//     string x;
//     cin >> x;
//     char* ip_addr;
//     ip_addr = (char *)x.c_str();
//     cout << proximity(ip_addr)<< endl;
// }