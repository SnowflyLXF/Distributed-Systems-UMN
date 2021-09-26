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
#include <sys/stat.h>

#define BUFLEN 1024

using namespace std;

uint32_t checksum(std::ifstream& file);

class Node
{
private:
    struct sockaddr_in _server_addr, _node_addr, _onlisten_addr, _tmp_addr;
    int _node_socket_fd,_onlisten_fd, _slen, _port, _onListenPort;
    char _buf[BUFLEN];
    typedef vector<string> names;
    names tmpfiles;
    int load;
    int num_up;
    int max_port;
    string dirname;

public:
    Node(int, int);
    void onListen();
    string listen2();
    int Connect();
    int SetServer(string, int);
    string listen();
    int sendall(char *, int *);
    int FileList();
    names getFileList();
    int Update();
    names Find(string);
    int GetLoad(string);
    int Download(string);
    void Uploader(string, int, string);
    int handleUpload(string, int, string);
    int handleDownload(string, string);
};