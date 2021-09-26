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
#include <vector>
#include <thread>
#include <unistd.h>
#include <dirent.h>

#define BUFLEN 1024

using namespace std;

class Node
{
private:
    struct sockaddr_in _server_addr, _node_addr;
    int _node_socket_fd, _slen, _port;
    char _buf[BUFLEN];
    typedef vector<string> names;
    names tmpfiles;

public:
    Node(int);
    int Connect();
    int SetServer(string, int);
    char *listen();
    int sendall(char *, int *);
    int FileList();
    int Update();
    int Find(string);
    int GetLoad(string);
    int Download(string);
    int Uploader();
};