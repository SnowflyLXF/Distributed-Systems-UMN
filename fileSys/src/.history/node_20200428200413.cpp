#include "node.hpp"
using namespace std;

uint32_t checksum(std::ifstream& file) 
{
    uint32_t sum = 0;

    uint32_t word = 0;
    while (file.read(reinterpret_cast<char*>(&word), sizeof(word))) {
        sum += word;
    }

    if (file.gcount()) {
        word &= (~0U >> ((sizeof(uint32_t) - file.gcount()) * 8));
        sum += word;
    }

    return sum;
}


Node ::Node(int Port, int OnPort)
{
    num_up = 0;
    load = 0;
    _port = Port;
    _onListenPort = OnPort;
    _slen = sizeof(_node_addr);
    max_port = OnPort+1;

    dirname = "node" + to_string(_port);

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
    strncpy(msg, _buf2, recvLen);
    // string s(msg);
    msg[recvLen] = '\0';
    string s(msg);
    // cout << s << endl;
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
        if(strcmp(m.c_str(), "GetLoad")==0)
        {
            char msg[BUFLEN] = {0};
            int len = sizeof(to_string(load).c_str());
            snprintf(msg, len, to_string(load).c_str());
            sendto(_onlisten_fd, msg, len, 0, (struct sockaddr *)&_tmp_addr, _slen);
        }
        else if (strcmp(m.c_str(), "Download")==0)
        {
            string dIP = listen2();
            int dport = stoi(listen2());
            string file = listen2();
            // cout << dIP << ":" << dport << ", "<< file << endl;
            handleUpload(dIP, dport, file);
        }
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
        tmpnodes.push_back(inmsg);
        cout<<inmsg<<endl;
    }
    // GetLoad(tmpnodes[0]);
    // GetLoad(tmpnodes[1]);


    return tmpnodes;
}

int Node::GetLoad(string c)
{
    char *p;
    char *strs = new char[c.length() + 1]; 
    strcpy(strs,c.c_str());

    p = strtok(strs, ":");
    string IP(p);
    p = strtok(NULL, ":");
    int p2 = atoi(p);
    // cout<<p2<<endl;

    struct sockaddr_in _c_addr;
    memset((char *)&_c_addr, 0, sizeof(_c_addr));
    _c_addr.sin_family = AF_INET;
    _c_addr.sin_addr.s_addr = inet_addr(IP.c_str());
    _c_addr.sin_port = htons(p2);

    char msg[BUFLEN] = {0};
    int len = sizeof("GetLoad");
    snprintf(msg, len, "GetLoad");
    sendto(_node_socket_fd, msg, len, 0, (struct sockaddr *)&_c_addr, _slen);

    string m = listen();
    cout<<m<<endl;

    // len = sizeof("Download");
    // snprintf(msg, len, "Download");
    // sendto(_node_socket_fd, msg, len, 0, (struct sockaddr *)&_c_addr, _slen);
    // len = sizeof("127.0.0.1");
    // snprintf(msg, len, "127.0.0.1");
    // sendto(_node_socket_fd, msg, len, 0, (struct sockaddr *)&_c_addr, _slen);
    // len = sizeof("6001");
    // snprintf(msg, len, "6001");
    // sendto(_node_socket_fd, msg, len, 0, (struct sockaddr *)&_c_addr, _slen);
    // len = sizeof("cnm.txt");
    // snprintf(msg, len, "cnm.txt");
    // sendto(_node_socket_fd, msg, len, 0, (struct sockaddr *)&_c_addr, _slen);

    // m = listen();
    // cout<<m<<endl;

    // m = listen();
    // cout << m << endl;
    return stoi(m);
}

int Node::Download(string file)
{
    Node::names upers = Find(file);
    string uper = upers.front();
    int minload = GetLoad(uper);
    for(int i=1; i<upers.size(); i++){
        int l = GetLoad(upers.at(i));
        if(l < minload) {
            uper = l;
            minload = GetLoad(uper);
        }
    }

    load++;
    thread onDownload(&Node::handleDownload, this, uper, file);
    onDownload.detach();
    // cout<<m<<endl;

}

int Node::handleDownload(string uper, string file){
    struct sockaddr_in down_node_addr, up_node_addr;
    memset((char *)&down_node_addr, 0, sizeof(down_node_addr));
    down_node_addr.sin_family = AF_INET;
    down_node_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    down_node_addr.sin_port = htons(max_port++);

    int down_node_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (::bind(down_node_socket_fd, (struct sockaddr *)&down_node_addr, sizeof(down_node_addr)) == -1)
    {
        cout << "Bind failed!" << endl;
    }

    int pos = uper.find(":");
    string upip = uper.substr(0,pos);
    string p = uper.substr(pos+1, uper.length()+1);
    int upport = stoi(p);

    memset((char *)&up_node_addr, 0, sizeof(up_node_addr));
    up_node_addr.sin_family = AF_INET;
    up_node_addr.sin_addr.s_addr = inet_addr(upip.c_str());
    up_node_addr.sin_port = htons(upport);

    char msg[BUFLEN] = {0};
    int len = sizeof("Download");
    snprintf(msg, len, "Download");
    sendto(down_node_socket_fd, msg, len, 0, (struct sockaddr *)&up_node_addr, _slen);
    len = sizeof(upip);
    snprintf(msg, len, upip.c_str());
    sendto(down_node_socket_fd, msg, len, 0, (struct sockaddr *)&up_node_addr, _slen);
    len = sizeof(p);
    snprintf(msg, len, p.c_str());
    sendto(down_node_socket_fd, msg, len, 0, (struct sockaddr *)&up_node_addr, _slen);
    len = sizeof(file);
    snprintf(msg, len, file.c_str());
    sendto(down_node_socket_fd, msg, len, 0, (struct sockaddr *)&up_node_addr, _slen);

    char _bufd[BUFLEN];
    int recvLen = recvfrom(down_node_socket_fd, _bufd, BUFLEN, 0, (struct sockaddr *)&_tmp_addr, reinterpret_cast<socklen_t *>(&_slen));

    char filed[recvLen + 1];
    strncpy(filed, _buf, recvLen);
    cout << "dsdadas" << m << endl;

    m = listen();
    cout << "ytr" << m << endl;

}

int Node::handleUpload(string dip, int dport, string file)
{
    thread uplder(&Node::Uploader, this, dip, dport, file);
    uplder.detach();
    num_up ++;

}

void Node::Uploader(string dip, int dport, string file)
{
    char msg[BUFLEN] = {0};
    int len;

    load++;
    struct sockaddr_in downloader_addr;
    int uploader_socket_fd;
    memset((char *)&downloader_addr, 0, sizeof(downloader_addr));
    downloader_addr.sin_family = AF_INET;
    downloader_addr.sin_addr.s_addr = inet_addr(dip.c_str()); 
    downloader_addr.sin_port = htons(dport);

    uploader_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    DIR *dir;
    struct dirent *ptr;
    FILE *fp;

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
        {
            if(strcmp(ptr->d_name, file.c_str()) == 0)
            {
                fp = fopen((dirname +"/"+ file).c_str(), "rb");
                if (fp == NULL)
                {
                    cerr << "File Open Error";
                }
                struct stat st;
                stat((dirname + "/" + file).c_str(), &st);
                size_t fsize = st.st_size;

                len = sizeof(fsize);
                snprintf(msg, len, "%d", fsize);
                sendto(uploader_socket_fd, msg, len, 0, (struct sockaddr *)&downloader_addr, _slen);


                fread(msg, 1, fsize, fp);
                sendto(uploader_socket_fd, msg, fsize, 0, (struct sockaddr *)&downloader_addr, _slen);
                
                fclose(fp);
            }
        }
    }
    closedir(dir);

    load--;
}
