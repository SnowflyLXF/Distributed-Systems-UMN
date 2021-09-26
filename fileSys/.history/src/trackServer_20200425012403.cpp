#include "server.hpp"
using namespace std;

trackServer ::trackServer(int Port)
{
    _port = Port;
    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _server_addr.sin_port = htons(_port);

    _server_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_track);

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
    cout << msg << endl;
    if (strcmp(msg, "Connect") == 0)
    {
        cout << "Connected by " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 0;
    }
    else if (strcmp(msg, "Read") == 0)
    {
        return 1;
    }
    else if (strcmp(msg, "Choose") == 0)
    {
        return 2;
    }
    else if (strcmp(msg, "Post") == 0)
    {
        return 3;
    }
    else if (strcmp(msg, "Reply") == 0)
    {
        return 4;
    }
    else if (strcmp(msg, "PPost") == 0)
    {
        return 5;
    }
    else if (strcmp(msg, "Update") == 0)
    {
        return 6;
    }
    else if (strcmp(msg, "PReply") == 0)
    {
        return 7;
    }
    else if (strcmp(msg, "Request") == 0)
    {
        return 8;
    }
    else if (strcmp(msg, "Sync") == 0)
    {
        return 9;
    }
}

string trackServer::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_server_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_client_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen];
    strncpy(msg, _buf, recvLen);
    string s(msg);

    return s;
}

int trackServer ::send(string msg)
{
    // cout<<msg.c_str()<<endl;
    try
    {
        int s = sendto(_server_socket_fd, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *)&_client_addr, _slen);
        // cout << s << endl;
    }
    catch (int exp)
    {
        cout << exp << endl;
    }
}

void trackServer::setReceiverPort(int Port)
{
    _client_addr.sin_port = htons(Port);
}