#include "node.hpp"
using namespace std;

Node ::Node(int Port)
{
    _port = Port;
    _slen = sizeof(_node_addr);

    memset((char *)&_node_addr, 0, sizeof(_node_addr));
    _node_addr.sin_family = AF_INET;
    _node_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _node_addr.sin_port = htons(_port);

    _node_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (::bind(_node_socket_fd, (struct sockaddr *)&_node_addr, sizeof(_node_addr)) == -1)
    {
        cout << "Bind failed!" << endl;
    }


}

int Node::Connect()
{
    char msg[BUFLEN] = {0};
    int len = sizeof("Connect");
    snprintf(msg, len, "Connect");
    int n = sendall(msg, &len);
    return n;
}

int Node::SetServer(string serverIP, int serverPort)
{
    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    _server_addr.sin_port = htons(serverPort);
}

char *Node::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_node_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_server_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen];
    strncpy(msg, _buf, recvLen);
    // string s(msg);

    return msg;
}

int Node ::sendall(char *msg, int *len)
{
    // cout<<msg.c_str()<<endl;
    int bytesLeft = *len;
    int sentLen = 0;
    int n;

    while (sentLen < *len)
    {
        n = sendto(_node_socket_fd, msg + sentLen, bytesLeft, 0, (struct sockaddr *)&_server_addr, _slen);
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

Node::names Node::getFileList()
{
    names files;
    DIR *dir;
    struct dirent *ptr;
    

}

int Node::FileList()
{
    char msg[BUFLEN] = {0};
    int len = sizeof("FileList");
    snprintf(msg, len, "FileList");
    int n = sendall(msg, &len);

    len = sizeof("1");
    snprintf(msg, len, "1");
    n = sendall(msg, &len);

    len = sizeof("sbsss");
    snprintf(msg, len, "sbsss");
    n = sendall(msg, &len);
}