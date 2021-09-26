#pragma once
#include <stdio.h> //printf
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <list>

#define BUFLEN 256

    using namespace std;

class UDPServer{
    private:
        struct sockaddr_in _server_addr, _client_addr;
        int _server_socket_fd, _client_socket_fd, _slen, _port;
        char _buf[BUFLEN];

    public:
        UDPServer(int);
        void setReceiverPort(int);
        int OnConnect();
        string listen();
        int send(string);
};
