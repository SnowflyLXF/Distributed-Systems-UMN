#include "trackServer.hpp"
using namespace std;

trackServer ::trackServer(int Port)
{
    _port = Port;
    _slen = sizeof(_client_addr);
    
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
    char * msg;

    msg = listen();

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

char * trackServer::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_server_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_client_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen];
    strncpy(msg, _buf, recvLen);
    // string s(msg);

    return msg;
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

int trackServer::handleCreate()
{   
    string ClientIP(inet_ntoa(_client_addr.sin_addr));
    string ClientPort = to_string(ntohs(_client_addr.sin_port));
    string ClientAddr = ClientIP + ":" + ClientIP;
    
    int filecount;
    char *msg;

    msg = listen();

    filecount = atoi(msg);

    for (int i = 0;i<filecount; i++)
    {
        msg = listen();
        tmpfiles.push_back(msg);
    }

    fileLists[ClientAddr] = tmpfiles;
    tmpfiles.clear();


}

int trackServer::handleFind()
{
    names nodeList;
    char * msg;
    char buff[BUFLEN] = {0};
    int listsize;
    int nodesize;

    msg = listen();

    map<string, names>::iterator iterm;

    for(iterm = fileLists.begin(); iterm!=fileLists.end();iterm++)
    {
        names l = iterm->second;
        listsize = l.size();
        for (int i=0; i<listsize;i++)
        {
            if(strcmp(msg, l[i].c_str()) == 0)
            {
                nodeList.push_back(iterm->first);
                break;
            }
        }
    }

    nodesize = nodeList.size();
    snprintf(buff, BUFLEN, "%d", nodesize);
    int sendsize = sizeof(buff);
    sendall(buff, &sendsize);

    memset(buff, '0', sizeof(buff));
    for (int i = 0; i < nodesize; i++)
    {
        snprintf(buff, BUFLEN, "%s", nodeList[i].c_str());
        int sendsize = sizeof(buff);
        sendall(buff, &sendsize);
        memset(buff, '0', sizeof(buff));
    }
}