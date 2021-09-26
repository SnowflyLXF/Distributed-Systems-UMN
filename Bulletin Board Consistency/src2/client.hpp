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
#include <vector>

#define BUFLEN 256

using namespace std;

class UDPClient
{
private:
    struct sockaddr_in _server_addr, _client_addr;
    int _server_socket_fd,  _client_socket_fd, _message_length, _port, _slen;
    char _buf[BUFLEN];

public:
    UDPClient(int);
    int send(string);
    int Connect(string, int);
    int Read();
    string listen();
    int Post();
    int Choose();
    int Reply();
};