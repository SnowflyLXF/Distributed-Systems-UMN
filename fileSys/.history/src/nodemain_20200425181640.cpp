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

int sendall(char *msg, int *len)
{
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
