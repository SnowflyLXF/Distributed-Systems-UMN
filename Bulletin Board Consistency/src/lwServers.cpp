#include "lwServers.hpp"
#include <pthread.h>
#include <map>

using namespace std;

vector<lwArticle> lwArtCollect; // Vector that contain all the articles published on this group server

lwServers ::lwServers(int Port, int isCo)
{
    _Port = Port;
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

int lwServers ::handleConnect()
{
    int result = _UDPServer->OnConnect();
    if (result == 1)
    {
        handleRead();
    }
    else if (result == 2)
    {
        handleChoose();
    }
    else if (result == 3)
    {
        handlePost();
    }
    else if (result == 4)
    {
        handleReply();
    }
    else if (result == 5)
    {
        // handlePPost();
    }
    else if (result == 6)
    {
        handleUpdate();
    }
    else if (result == 7)
    {
        // handlePReply();
    }
    else if (result == 8)
    {
        handleIDRequest();
    }
    else if (result == 9)
    {
        handleSync();
    }
}

int lwServers ::handleChoose()
{
    _UDPServer->send("Which article to Read?");

    string idstr = _UDPServer->listen();
    stringstream m(idstr);
    int idx = 0;
    m >> idx;

    _UDPServer->send(lwArtCollect[idx - 1].title);
    _UDPServer->send(lwArtCollect[idx - 1].content);
    // int id = atoi(_UDPServer->listen());
}

int lwServers ::handleRead()
{
    _UDPServer->send(to_string(lwArtCollect.size()));
    for (int i = 0; i < lwArtCollect.size(); i++)
    {
        string msg = to_string(lwArtCollect[i].idx) + lwArtCollect[i].title;
        msg.insert(0, 5 * lwArtCollect[i].indent, ' ');
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

int lwServers ::handlePost()
{
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();

    if (_isCoordinator == 1)
    {

        lwArticle new_a;
        new_a.idx = lwArtCollect.size() + 1;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = 0;
        lwArtCollect.push_back(new_a);
        this->update();
    }
    else
    {

        this->sync();
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("Request");
        string idstr = _UDPServer->listen();
        lwArticle new_a;
        stringstream tmp(idstr);
        tmp >> new_a.idx;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = 0;
        lwArtCollect.push_back(new_a);
        _isCoordinator = 1;
        vector<int>::iterator iter = find(_servers.begin(), _servers.end(), _Port);
        if (iter != _servers.end())
            _servers.erase(iter);
        _servers.push_back(_coordPort);
        this->update();
    }
}

int lwServers::update()
{
    int id = lwArtCollect.size() - 1;
    string idstr = to_string(lwArtCollect[id].idx);
    string tt = lwArtCollect[id].title;
    string ct = lwArtCollect[id].content;
    string indentstr = to_string(lwArtCollect[id].indent);
    for (int n : _servers)
    {
        _UDPServer->setReceiverPort(n);
        _UDPServer->send("Update");
        _UDPServer->send(idstr);
        _UDPServer->send(tt);
        _UDPServer->send(ct);
        _UDPServer->send(indentstr);
        _UDPServer->send(to_string(_Port));
    }
}

int lwServers::handleUpdate()
{
    string idstr = _UDPServer->listen();
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string indentstr = _UDPServer->listen();

    string coportstr = _UDPServer->listen();
    stringstream tmp3(coportstr);
    tmp3 >> _coordPort;

    lwArticle new_a;
    stringstream tmp(idstr);
    tmp >> new_a.idx;
    stringstream tmp2(indentstr);
    tmp2 >> new_a.indent;
    new_a.title = tt;
    new_a.content = ct;

    lwArtCollect.push_back(new_a);
}

int lwServers::handleReply()
{
    string idstr = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string tt = " A reply to Article " + idstr;

    int rid = 0;
    stringstream tmp(idstr);
    tmp >> rid;

    if (_isCoordinator == 1)
    {
        lwArticle new_a;
        new_a.idx = lwArtCollect.size() + 1;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = lwArtCollect[rid - 1].indent + 1;
        lwArtCollect.push_back(new_a);
        this->update();
    }
    else
    {
        this->sync();
        _UDPServer->setReceiverPort(_coordPort);

        _UDPServer->send("Request");
        string idstr = _UDPServer->listen();
        lwArticle new_a;
        stringstream tmp(idstr);
        tmp >> new_a.idx;
        new_a.title = tt;
        new_a.content = ct;
        new_a.indent = lwArtCollect[rid - 1].indent + 1;
        lwArtCollect.push_back(new_a);
        _isCoordinator = 1;
        vector<int>::iterator iter = find(_servers.begin(), _servers.end(), _Port);
        if (iter != _servers.end())
            _servers.erase(iter);
        _servers.push_back(_coordPort);
        this->update();
    }
}

int lwServers::handleIDRequest()
{
    _UDPServer->send(to_string(lwArtCollect.size() + 1));
    _isCoordinator = 0;
}

int lwServers::sync()
{
    _UDPServer->setReceiverPort(_coordPort);
    _UDPServer->send("Sync");
    string newVerstr = _UDPServer->listen();
    int newVer = 0;
    stringstream tmp(newVerstr);
    tmp >> newVer;
    int diff = newVer - lwArtCollect.size();
    _UDPServer->send(to_string(diff));

    if (newVer != lwArtCollect.size())
    {

        for (int i = 0; i < diff; i++)
        {
            string idstr = _UDPServer->listen();
            string tt = _UDPServer->listen();
            string ct = _UDPServer->listen();
            string indentstr = _UDPServer->listen();

            lwArticle new_a;
            stringstream tmp(idstr);
            tmp >> new_a.idx;
            stringstream tmp2(indentstr);
            tmp2 >> new_a.indent;
            new_a.title = tt;
            new_a.content = ct;

            lwArtCollect.push_back(new_a);
        }
    }
}

int lwServers::handleSync()
{
    _UDPServer->send(to_string(lwArtCollect.size()));
    int diff = 0;
    string diffstr = _UDPServer->listen();
    stringstream tmp(diffstr);
    tmp >> diff;
    if (diff > 0)
    {
        for (int id = diff; id > 0; id--)
        {
            string idstr = to_string(lwArtCollect[id].idx);
            string tt = lwArtCollect[id].title;
            string ct = lwArtCollect[id].content;
            string indentstr = to_string(lwArtCollect[id].indent);

            _UDPServer->send(idstr);
            _UDPServer->send(tt);
            _UDPServer->send(ct);
            _UDPServer->send(indentstr);
        }
    }
}