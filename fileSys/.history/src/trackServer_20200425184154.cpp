#include "trackServer.hpp"
using namespace std;

trackServer ::trackServer(int Port)
{
    _port = Port;
    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _server_addr.sin_port = htons(_port);

    _server_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (::bind(_server_socket_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) == -1)
    {
        cout << "Bind failed!" << endl;
    }
}

int trackServer::OnConnect()
{
    _slen = sizeof(_client_addr);

    memset(_buf, ' ', BUFLEN);

    int recvLen = recvfrom(_server_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_client_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen];
    strncpy(msg, _buf, recvLen);
    if (strcmp(msg, "Connect") == 0)
    {
        cout << "Connected by " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 0;
    }
    else if (strcmp(msg, "FileList") == 0)
    {
        cout << "Creating file list from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 1;
    }
    else if (strcmp(msg, "Find") == 0)
    {
        cout << "Find Request from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 2;
    }
    else if (strcmp(msg, "UpdateList") == 0)
    {
        cout << "Updating file list from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 3;
    }
}

int trackServer ::sendall(char * msg, int * len)
{
    // cout<<msg.c_str()<<endl;
    int bytesLeft = * len;
    int sentLen = 0;
    int n;

    while(sentLen< *len)
    {
        n = sendto(_server_socket_fd, msg+sentLen, bytesLeft, 0, (struct sockaddr *)&_client_addr, _slen);
        if (n==-1)
        {
            break;
        }
        sentLen += n;
        bytesLeft -= n;
    }
    *len = sentLen;
    return n==-1 ? -1:0;
}
