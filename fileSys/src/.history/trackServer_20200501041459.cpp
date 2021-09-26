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

    thread beat(&trackServer::heartbeat, this);
    beat.detach();
}

int trackServer::heartbeat()
{
    map<string, names>::iterator iterm;
    memset((char *)&_tmp_addr, 0, sizeof(_tmp_addr));
    int _slen_tmp = sizeof(_tmp_addr);
    
    while(1)
    {
        for (iterm = fileLists.begin(); iterm != fileLists.end(); iterm++)
        {
            string c = iterm->first;
            char *p;
            char *strs = new char[c.length() + 1];
            strcpy(strs, c.c_str());

            p = strtok(strs, ":");
            string IP(p);
            p = strtok(NULL, ":");
            int p2 = atoi(p);
            _tmp_addr.sin_family = AF_INET;
            _tmp_addr.sin_addr.s_addr = inet_addr(IP.c_str());
            _tmp_addr.sin_port = htons(p2);

            char msg[BUFLEN] = {0};
            int len = sizeof("heartbeat");
            snprintf(msg, len, "heartbeat");
            sendto(_server_socket_fd, msg, len, 0, (struct sockaddr *)&_tmp_addr, _slen_tmp);
        }
        sleep(2);
    }
    
}

int trackServer::OnConnect()
{
    string msg;

    msg = listen();

    if (strcmp(msg.c_str(), "Connect") == 0)
    {
        cout << "Connected by " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        return 0;
    }
    else if (strcmp(msg.c_str(), "FileList") == 0)
    {
        cout << "Creating file list from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        handleCreate();
        return 1;
    }
    else if (strcmp(msg.c_str(), "Find") == 0)
    {
        cout << "Find Request from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        handleFind();
        return 2;
    }
    else if (strcmp(msg.c_str(), "Update") == 0)
    {
        cout << "Updating file list from " << inet_ntoa(_client_addr.sin_addr) << ":" << ntohs(_client_addr.sin_port) << endl;
        handleUpdate();
        return 3;
    }
}

string trackServer::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_server_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_client_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen+1];
    // cout<<_buf<<endl;
    strncpy(msg, _buf, recvLen);
    // string s(msg);
    msg[recvLen] = '\0';
    string s(msg);

    return s;
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
    string dport, hbport, msg;
    dport = listen();
    hbport = listen();

    string ClientAddr = ClientIP + ":" + dport;
    string hbAddr = ClientIP + ":" + hbport;
    hbList.push_back(hbAddr);

    int filecount;
    msg = listen();
    cout << msg<< endl;
    filecount = stoi(msg);

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
    string msg;
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
            if(strcmp(msg.c_str(), l[i].c_str()) == 0)
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

int trackServer::handleUpdate()
{
    string ClientIP(inet_ntoa(_client_addr.sin_addr));
    string ClientPort = to_string(ntohs(_client_addr.sin_port));
    string dport, hbport, msg;
    dport = listen();
    hbport = listen();

    string ClientAddr = ClientIP + ":" + dport;
    string hbAddr = ClientIP + ":" + hbport;
    hbList.push_back(hbAddr);

    fileLists.erase(ClientAddr);

    int filecount;
    string msg;

    msg = listen();

    filecount = stoi(msg);

    for (int i = 0; i < filecount; i++)
    {
        msg = listen();
        tmpfiles.push_back(msg);
    }

    fileLists[ClientAddr] = tmpfiles;
    tmpfiles.clear();
}