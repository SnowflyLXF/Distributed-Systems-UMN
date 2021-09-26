#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <list>

using namespace std;

#define BUFLEN 1024

struct sockaddr_in _server_addr, _client_addr;
int _server_socket_fd;


int main()
{
    char msg[BUFLEN] = {0};
    int *len;

    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _server_addr.sin_port = htons(_port);

    _server_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // cout<<msg.c_str()<<endl;
    int bytesLeft = *len;
    int sentLen = 0;
    int n;

    while (sentLen < *len)
    {
        n = sendto(_server_socket_fd, msg + sentLen, bytesLeft, 0, (struct sockaddr *)&_client_addr, _slen);
        if (n == -1)
        {
            break;
        }
        sentLen += n;
        bytesLeft -= n;
    }
    *len = sentLen;
    return n == -1 ? -1 : 0;
}
