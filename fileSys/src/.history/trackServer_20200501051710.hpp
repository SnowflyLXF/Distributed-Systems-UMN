#pragma once
#include <stdio.h> 
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <list>
#include <vector>
#include <map>
#include <thread>
#include <string.h>

#define BUFLEN 1024

using namespace std;

class trackServer
{
private:
    struct sockaddr_in _server_addr, _client_addr, _tmp_addr;
    int _server_socket_fd, _client_socket_fd, _slen, _port;
    char _buf[BUFLEN];
    typedef vector<string> names;
    map<string, names> fileLists;
    names tmpfiles;
    names hbList;

public:
    trackServer(int);
    int OnConnect();
    string listen();
    int sendall(char *, int *);
    int handleCreate(string);
    int handleFind();
    int handleUpdate(string);
    int heartbeat();
};
