#include "pbServers.hpp"
#include <pthread.h>
#include <map>

using namespace std;

vector<Article> ArtCollect;           // Vector that contain all the articles published on this group server

pbServers ::pbServers(int Port, int isCo)
{
    _UDPServer = new UDPServer(Port);
    _isCoordinator = isCo;
    _uniqID = 0;
    _coordPort = 5050;
    _servers.push_back(5046);
    _servers.push_back(5047);
    _servers.push_back(5048);
    _servers.push_back(5049);

        strcpy(_directname, "pbServer_");
    strcat(_directname, to_string(Port).c_str());

    if (mkdir(_directname, 0777) == -1)
        cout << "Directory existed\n";
    else
        cout << "Directory created\n";
}

int pbServers ::handleConnect()
{   
    int result = _UDPServer->OnConnect();
    if(result == 1){
        handleRead();
    }
    else if(result == 2)
    {
        handleChoose();
    }
    else if (result ==3)
    {
        handlePost();
    }
    else if (result ==4)
    {
        handleReply();
    }
    else if (result == 5)
    {
        handlePPost();
    }
    else if (result == 6)
    {
        handleUpdate();
    }
    else if (result == 7)
    {
        handlePReply();
    }

}

int pbServers :: handleChoose()
{
    _UDPServer->send("Which article to Read?");

    string idstr = _UDPServer->listen();
    stringstream m(idstr);
    int idx = 0;
    m >> idx;

    _UDPServer->send(ArtCollect[idx-1].title);
    _UDPServer->send(ArtCollect[idx-1].content);
    // int id = atoi(_UDPServer->listen());


}

int pbServers :: handleRead()
{
    _UDPServer->send(to_string(ArtCollect.size()));
    for(int i=0; i<ArtCollect.size(); i++){
        string msg = to_string(ArtCollect[i].idx) + ArtCollect[i].title;
        msg.insert(0, 5 * ArtCollect[i].indent, ' ');
        _UDPServer->send(msg);
    }

    // DIR *dp;
    // struct dirent *dirp;
    // regex reg_obj("[0-9]+\..*", regex::icase);
    // vector<string> titles;

    // if ((dp = opendir(_directname)) == NULL)
    //     cout << "Can't open " << _directname << endl;

    // while ((dirp = readdir(dp)) != NULL)
    //     if (regex_match(dirp->d_name, reg_obj))
    //         // cout << dirp->d_name << endl;
    //         titles.push_back(dirp->d_name);

    // _UDPServer->send(to_string(titles.size()));
    // for(string n : titles){
    //     _UDPServer->send(n);
    // }

    
}

int pbServers :: handlePost()
{
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();

    if (_isCoordinator == 1)
    {
        
        Article new_a;
        new_a.idx = ArtCollect.size() + 1;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = 0;
        ArtCollect.push_back(new_a);
        this->update();
    }
    else{
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("PPost");
        _UDPServer->send(tt);
        _UDPServer->send(ct);

    }
    
}

int pbServers::update()
{
    int id = ArtCollect.size() - 1;
    string idstr = to_string(ArtCollect[id].idx);
    string tt = ArtCollect[id].title;
    string ct = ArtCollect[id].content;
    string indentstr = to_string(ArtCollect[id].indent);
    for (int n:_servers){
        _UDPServer->setReceiverPort(n);
        _UDPServer->send("Update");
        _UDPServer->send(idstr);
        _UDPServer->send(tt);
        _UDPServer->send(ct);
        _UDPServer->send(indentstr);
    }
}

int pbServers::handlePPost()
{
    handlePost();
}

int pbServers::handleUpdate()
{
    string idstr = _UDPServer->listen();
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string indentstr = _UDPServer->listen();

    Article new_a;
    stringstream tmp(idstr);
    tmp >> new_a.idx;
    stringstream tmp2(indentstr);
    tmp2 >> new_a.indent;
    new_a.title = tt;
    new_a.content = ct;

    ArtCollect.push_back(new_a);
    
}

int pbServers::handleReply()
{
    string idstr = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string tt = " A reply to Article " + idstr;

    int rid = 0;
    stringstream tmp(idstr);
    tmp >> rid;

    

    if (_isCoordinator == 1)
    {
        Article new_a;
        new_a.idx = ArtCollect.size() + 1;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = ArtCollect[rid-1].indent + 1;
        ArtCollect.push_back(new_a);
        this->update();
    }
    else
    {
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("PReply");
        _UDPServer->send(idstr);
        _UDPServer->send(ct);
    }
}

int pbServers::handlePReply()
{
    handleReply();
}