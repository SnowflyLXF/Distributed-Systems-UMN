#pragma once
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

#define BUFLEN 1024

using namespace std;

class trackServer
{
private:
    struct sockaddr_in _server_addr, _client_addr;
    int _server_socket_fd, _client_socket_fd, _slen, _port;
    char _buf[BUFLEN];

public:
    trackServer(int);
    int OnConnect();
    int sendall(char *, int *);
};
