#include "client.hpp"

using namespace std;

UDPClient :: UDPClient(int Port)
{
    _port = Port;
    _client_addr.sin_family = AF_INET;
    _client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _client_addr.sin_port = htons(_port);
    _client_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_client_socket_fd==-1)
    {
        cout << "socket failed!" << endl;
    }
    int bind_res = ::bind(_client_socket_fd, (struct sockaddr *)&_client_addr, sizeof(_client_addr));
}

int UDPClient :: send(string msg)
{
    // cout<<msg.c_str()<<endl;
    try{
        int s = sendto(_client_socket_fd, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *)&_server_addr, _slen);
        // cout<<s<<endl;
    }
    catch (int exp)
    {
        cout<<exp<<endl;
    }
}

string UDPClient::listen()
{
    memset(_buf, ' ', BUFLEN);
    int recvLen = recvfrom(_client_socket_fd, _buf, BUFLEN, 0, (struct sockaddr *)&_server_addr, reinterpret_cast<socklen_t *>(&_slen));

    char msg[recvLen];
    strncpy(msg, _buf, recvLen);
    string str;

    return msg;
}

int UDPClient :: Connect(string to_addr, int to_port){

    memset((char *)&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(to_port);
    _server_addr.sin_addr.s_addr = inet_addr(to_addr.c_str());
    _slen = sizeof(_server_addr);
    this->send("Connect");

    // connect(_client_socket_fd, (struct sockaddr *)&_server_addr, _slen);
}

int UDPClient :: Read()
{
    this->send("Read");
    string msg;
    msg = this->listen();

    stringstream m(msg);
    int num = 0;
    m >> num;
    cout << num << endl;
    // int num = atoi(msg);
    // cout<<num<<endl;
    if (num<5){
        for (int i = 0; i < num; i++)
        {
            msg = this->listen();
            cout << msg << endl;
        }
    }
    
}

int UDPClient::Choose()
{
    this->send("Choose");
    
    string msg = this->listen();
    string idstr;
    if (msg.compare("Which article to Read?")==0) {
        cout << msg;
        getline(cin, idstr);
    }
    this->send(idstr);

    msg = this->listen();
    cout << "title: " << msg << endl;
    msg = this->listen();
    cout << "content: " << msg << endl;

}

int UDPClient::Post()
{

    this->send("Post");
    this->send("sb");
    this->send("ajoy");
    // cout << "title: ";
    // string t1;
    // getline(cin, t1);
    // this->send(t1);

    // cout << "content: ";
    // string t2;
    // getline(cin, t2);
    // this->send(t2);
}

int UDPClient::Reply()
{
    this->send("Reply");
    this->send("1");
    this->send("good");
}



int main()
{
    UDPClient c1(5051);
    c1.Connect("127.0.0.1", 5050);

    // c1.Post();
    string cm;
    while(1){
        cout << "Enter command: ";
        getline(cin, cm);
        if (cm.compare("read")==0){
            c1.Read();
        }
        else if (cm.compare("post") == 0){
                c1.Post();
            
            }
        else if (cm.compare("reply") == 0){
            c1.Reply();
        }
        else if (cm.compare("choose") == 0){
            c1.Choose();
        }
    }
}
