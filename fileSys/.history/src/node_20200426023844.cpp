#include "node.hpp"
using namespace std;

Node ::Node(int Port, int OnPort)
{
    _port = Port;
    _onListenPort = OnPort;
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

    thread onLis(&Node::onListen, this);
    onLis.detach();

}
string Node::listen2()
{
    char _buf2[BUFLEN];
    memset(_buf2, ' ', BUFLEN);
    int recvLen = recvfrom(_onlisten_fd, _buf2, BUFLEN, 0, (struct sockaddr *)&_tmp_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen + 1];
    // cout<<_buf<<endl;
    strncpy(msg, _buf, recvLen);
    // string s(msg);
    msg[recvLen] = '\0';
    string s(msg);

    return s;
}
void Node::onListen()
{
    memset((char *)&_onlisten_addr, 0, sizeof(_onlisten_addr));
    memset((char *)&_tmp_addr, 0, sizeof(_tmp_addr));
    _onlisten_addr.sin_family = AF_INET;
    _onlisten_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _onlisten_addr.sin_port = htons(_onListenPort);

    _onlisten_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (::bind(_onlisten_fd, (struct sockaddr *)&_onlisten_addr, sizeof(_onlisten_addr)) == -1)
    {
        cout << "Bind failed!" << endl;
    }
    string m;
    while(1)
    {
        m = listen2();
        cout<<m<<endl;
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

string Node::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_node_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_server_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen + 1];
    // cout<<_buf<<endl;
    strncpy(msg, _buf, recvLen);
    // string s(msg);
    msg[recvLen] = '\0';
    string s(msg);

    return s;
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

    string dirname = "node" + to_string(_port);

    if ((dir = opendir(dirname.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8)
            files.push_back(ptr->d_name);
        
    }
    closedir(dir);
    return files;

}

int Node::FileList()
{
    char msg[BUFLEN] = {0};
    int len = sizeof("FileList");
    snprintf(msg, len, "FileList");
    int n = sendall(msg, &len);

    len = sizeof(to_string(_onListenPort));
    snprintf(msg, len, to_string(_onListenPort).c_str());
    n = sendall(msg, &len);

    names files = getFileList();
    int fsize = files.size();

    len = sizeof(to_string(fsize));
    snprintf(msg, len, to_string(fsize).c_str());
    n = sendall(msg, &len);

    for(int i = 0;i<fsize;i++)
    {
        len = sizeof(files[i]);
        snprintf(msg, len, files[i].c_str());
        n = sendall(msg, &len);
    }
    
}

int Node::Update()
{
    char msg[BUFLEN] = {0};
    int len = sizeof("Update");
    snprintf(msg, len, "Update");
    int n = sendall(msg, &len);

    names files = getFileList();
    int fsize = files.size();

    len = sizeof(to_string(fsize));
    snprintf(msg, len, to_string(fsize).c_str());
    n = sendall(msg, &len);

    for (int i = 0; i < fsize; i++)
    {
        len = sizeof(files[i]);
        snprintf(msg, len, files[i].c_str());
        n = sendall(msg, &len);
    }
}

Node::names Node::Find(string filename)
{
    names tmpnodes;
    char msg[BUFLEN] = {0};
    int len = sizeof("Find");
    snprintf(msg, len, "Find");
    int n = sendall(msg, &len);

    len = sizeof(filename.c_str());
    snprintf(msg, len+1, filename.c_str());
    n = sendall(msg, &len);

    string inmsg;
    inmsg = listen();
    int nodecount = stoi(inmsg);


    for (int i = 0; i < nodecount; i++)
    {
        inmsg = listen();
        tmpnodes.push_back(msg);
    }
    return tmpnodes;
}