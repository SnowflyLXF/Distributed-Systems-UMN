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
int _slen = sizeof(_client_addr);

int main()
{
    char msg[BUFLEN] = {0};
    int *len;

    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _server_addr.sin_port = htons(5151);

    

    memset((char *)&_client_addr, 0, sizeof(_client_addr));
    _client_addr.sin_family = AF_INET;
    _client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    _client_addr.sin_port = htons(5050);

    _server_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (::bind(_server_socket_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
    {
        cout << "Bind failed!" << endl;
    }

    int tp = 10;
    len = &tp;
    // snprintf(msg, *len, "Connect");
    // cout<<msg.c_str()<<endl;
    int bytesLeft = *len;
    int sentLen = 0;
    int n;

    // while (sentLen < *len)
    // {
    //     n = sendto(_server_socket_fd, msg + sentLen, bytesLeft, 0, (struct sockaddr *)&_client_addr, _slen);
    //     if (n == -1)
    //     {
    //         break;
    //     }
    //     sentLen += n;
    //     bytesLeft -= n;
    // }

    snprintf(msg, *len, "FileList");

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
    cout<<sentLen<<endl;
    sentLen = 0;

    snprintf(msg, *len, "1");

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
    sentLen = 0;

    snprintf(msg, *len, "sb");

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
    sentLen = 0;
}
