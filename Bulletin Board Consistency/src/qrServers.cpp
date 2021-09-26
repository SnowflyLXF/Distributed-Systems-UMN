#include "qrServers.hpp"
#include <pthread.h>
#include <map>

using namespace std;

vector<qrArticle> qrArtCollect; // Vector that contain all the articles published on this group server

qrServers ::qrServers(int Port, int isCo, int Nr, int Nw)
{
    _Port = Port;
    _UDPServer = new UDPServer(Port);
    _isCoordinator = isCo;
    _Nr = Nr;
    _Nw = Nw;
    _writing = 0;
    _reading = 0;
    _uniqID = 0;
    _coordPort = 5050;
    // _servers.push_back(5046);
    // _servers.push_back(5047);
    // _servers.push_back(5048);
    // _servers.push_back(5049);
    // _servers.push_back(5050);

    strcpy(_directname, "pbServer_");
    strcat(_directname, to_string(Port).c_str());

    if (mkdir(_directname, 0777) == -1)
        cout << "Directory existed\n";
    else
        cout << "Directory created\n";
}

int qrServers ::handleConnect()
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
    else if (result == 10)
    {
        handleReqnr();
    }
    else if (result == 11)
    {
        handleReqnw();
    }
    else if (result == 12)
    {
        handleVoter();
    }
    else if (result == 13)
    {
        handleVotew();
    }
    else if(result == 14)
    {
        handleDoner(_Nr);
    }
    else if (result == 15)
    {
        // handleDonew(_Nw);
    }
    else if (result == 16)
    {
        handleDr();
    }
    else if (result == 17)
    {
        // handleDw();
    }
    else if (result == 18)
    {
        handlePostnw();
    }
    else if (result == 19)
    {
        handleReplynw();
    }
}

int qrServers ::handleChoose()
{
    _UDPServer->send("Which article to Read?");

    string idstr = _UDPServer->listen();
    stringstream m(idstr);
    int idx = 0;
    m >> idx;
    int nr = 0;
    int clientPort = ntohs(_UDPServer->_client_addr.sin_port);
    if (_isCoordinator != 1)
    {

        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("reqNr");
        _UDPServer->send(to_string(_Nr));
        string nrstr = _UDPServer->listen();
        stringstream m(nrstr);
        m >> nr;
    }
    else
    {
        nr = generateNr(_Nr);
    }

    if (nr == _Nr)
    {
        _UDPServer->setReceiverPort(clientPort);

        _UDPServer->send(qrArtCollect[idx - 1].title);
        _UDPServer->send(qrArtCollect[idx - 1].content);
        // int id = atoi(_UDPServer->listen());
    }
    else
    {
        _UDPServer->send("Reading command currently unavailable.");
    }

    if (_isCoordinator != 1)
    {
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("doner");
    }
    else
    {
        handleDoner(_Nr);
    }

}

int qrServers ::handleRead()
{
    int nr = 0;
    int clientPort = ntohs(_UDPServer->_client_addr.sin_port);
    if(_isCoordinator != 1){
       
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("reqNr");
        _UDPServer->send(to_string(_Nr));
        string nrstr = _UDPServer->listen();
        stringstream m(nrstr);
        m >> nr;
    }
    else
    {
        nr = generateNr(_Nr);
        
    }
    if(nr == _Nr)
    {
        _UDPServer->setReceiverPort(clientPort);
        _UDPServer->send(to_string(qrArtCollect.size()));
        for (int i = 0; i < qrArtCollect.size(); i++)
        {
            string msg = to_string(qrArtCollect[i].idx) + qrArtCollect[i].title;
            msg.insert(0, 5 * qrArtCollect[i].indent, ' ');
            _UDPServer->send(msg);
        }
    }
    else
    {
        _UDPServer->send("Reading command currently unavailable.");
    }

    if (_isCoordinator != 1)
    {
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("doner");
        
    }
    else
    {
        handleDoner(_Nr);
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

int qrServers ::handlePost()
{
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    int nw = 0;

    if(_isCoordinator != 1)
    {
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("reqNw");
        _UDPServer->send(to_string(_Nw));
        string nwstr = _UDPServer->listen();
        stringstream m(nwstr);
        m >> nw;
    }
    else{
        nw = generateNw(_Nw);
    }

    if(nw == _Nw)
    {
        if (_isCoordinator != 1)
        {
            _UDPServer->setReceiverPort(_coordPort);
            _UDPServer->send("postNw");
            _UDPServer->send(tt);
            _UDPServer->send(ct);
        }
        else
        {
            string idx = to_string(qrArtCollect.size() + 1);
            string indent = to_string(0);
            for (int i = 0; i < _Nw - 1; i++)
            {
                if (_Nreaders[i] != _coordPort)
                {
                    _UDPServer->setReceiverPort(_Nwriters[i]);
                    _UDPServer->send("Update");
                    _UDPServer->send(idx);
                    _UDPServer->send(tt);
                    _UDPServer->send(ct);
                    _UDPServer->send(indent);
                }
            }
            qrArticle new_a;
            new_a.idx = qrArtCollect.size() + 1;
            new_a.indent = 0;
            new_a.title = tt;
            new_a.content = ct;

            qrArtCollect.push_back(new_a);
            _writing = 0;
            this->sync();
        }
        

    }

    
}

int qrServers::update()
{
    int id = qrArtCollect.size() - 1;
    string idstr = to_string(qrArtCollect[id].idx);
    string tt = qrArtCollect[id].title;
    string ct = qrArtCollect[id].content;
    string indentstr = to_string(qrArtCollect[id].indent);
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

int qrServers::handleUpdate()
{
    string idstr = _UDPServer->listen();
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string indentstr = _UDPServer->listen();

    string coportstr = _UDPServer->listen();
    stringstream tmp3(coportstr);
    tmp3 >> _coordPort;

    qrArticle new_a;
    stringstream tmp(idstr);
    tmp >> new_a.idx;
    stringstream tmp2(indentstr);
    tmp2 >> new_a.indent;
    new_a.title = tt;
    new_a.content = ct;

    qrArtCollect.push_back(new_a);
}

int qrServers::handleReply()
{
    string idstr = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string tt = " A reply to Article " + idstr;

    int rid = 0;
    stringstream tmp(idstr);
    tmp >> rid;
    int indent = qrArtCollect[rid - 1].indent + 1;

    int newid = qrArtCollect.size()+1;

    int nw = 0;

    if (_isCoordinator != 1)
    {
        _UDPServer->setReceiverPort(_coordPort);
        _UDPServer->send("reqNw");
        _UDPServer->send(to_string(_Nw));
        string nwstr = _UDPServer->listen();
        stringstream m(nwstr);
        m >> nw;
    }
    else
    {
        nw = generateNw(_Nw);
    }

    if (nw == _Nw)
    {
        if (_isCoordinator != 1)
        {
            _UDPServer->setReceiverPort(_coordPort);
            _UDPServer->send("replyNw");
            _UDPServer->send(to_string(newid));
            _UDPServer->send(tt);
            _UDPServer->send(ct);
            _UDPServer->send(to_string(indent));
        }
        else
        {
            for (int i = 0; i < _Nw - 1; i++)
            {
                if (_Nreaders[i] != _coordPort)
                {
                    _UDPServer->setReceiverPort(_Nwriters[i]);
                    _UDPServer->send("Update");
                    _UDPServer->send(to_string(newid));
                    _UDPServer->send(tt);
                    _UDPServer->send(ct);
                    _UDPServer->send(to_string(indent));
                }
            }

            qrArticle new_a;
            new_a.idx= newid;
            new_a.indent = indent;
            new_a.title = tt;
            new_a.content = ct;

            qrArtCollect.push_back(new_a);
            _writing = 0;
            this->sync();
        }
        
    }

}

int qrServers::handleIDRequest()
{
    _UDPServer->send(to_string(qrArtCollect.size() + 1));
    _isCoordinator = 0;
}

int qrServers::sync()
{
    for (int p:_servers){
        _UDPServer->setReceiverPort(p);
        _UDPServer->send("Sync");
        string newVerstr = _UDPServer->listen();
        int newVer = 0;
        stringstream tmp(newVerstr);
        tmp >> newVer;
        int diff = qrArtCollect.size() - newVer;
        _UDPServer->send(to_string(diff));

        if (diff > 0)
        {

            for (int i = 0; i < diff; i++)
            {
                string idstr = _UDPServer->listen();
                string tt = _UDPServer->listen();
                string ct = _UDPServer->listen();
                string indentstr = _UDPServer->listen();

                qrArticle new_a;
                stringstream tmp(idstr);
                tmp >> new_a.idx;
                stringstream tmp2(indentstr);
                tmp2 >> new_a.indent;
                new_a.title = tt;
                new_a.content = ct;

                qrArtCollect.push_back(new_a);
            }
        }
    }
    
}

int qrServers::handleSync()
{
    _UDPServer->send(to_string(qrArtCollect.size()));
    int diff = 0;
    string diffstr = _UDPServer->listen();
    stringstream tmp(diffstr);
    tmp >> diff;
    if (diff > 0)
    {
        for (int id = diff; id > 0; id--)
        {
            string idstr = to_string(qrArtCollect[id].idx);
            string tt = qrArtCollect[id].title;
            string ct = qrArtCollect[id].content;
            string indentstr = to_string(qrArtCollect[id].indent);

            _UDPServer->send(idstr);
            _UDPServer->send(tt);
            _UDPServer->send(ct);
            _UDPServer->send(indentstr);
        }
    }
}

int qrServers::generateNr(int num)
{
    int nr = 0;

    _Nreaders = _servers;
    random_shuffle(_Nreaders.begin(), _Nreaders.end());
    for (int i =0;i<num-1;i++)
    {
            _UDPServer->setReceiverPort(_Nreaders[i]);
            _UDPServer->send("voter");
            string v = _UDPServer->listen();

            if (v.compare("y"))
            {
                nr++;
            }
        
        
    }
    if (_writing == 0)
    {
        nr++;
        _reading = 1;
    }

    return nr;

}

int qrServers::handleVoter()
{
    if (_writing ==1)
    {
        _UDPServer->send("n");
    }
    else
    {
        _UDPServer->send("y");
        _reading = 1;
    }
}

int qrServers::generateNw(int num)
{
    int nw = 0;
    _Nwriters = _servers;
    random_shuffle(_Nwriters.begin(), _Nwriters.end());
    for (int i = 0; i < num-1; i++)
    {
        
        _UDPServer->setReceiverPort(_Nwriters[i]);
        _UDPServer->send("voter");
        string v = _UDPServer->listen();

        if (v.compare("y"))
        {
            nw++;
        }
        
    }

    if (_reading == 0)
        {
            nw++;
            _writing = 1;
        }
    

    return nw;
}

int qrServers::handleVotew()
{
    if (_reading == 1)
    {
        _UDPServer->send("n");
    }
    else
    {
        _UDPServer->send("y");
        _writing = 1;
    }
}

int qrServers::handleDoner(int num)
{
    for (int i = 0; i < num-1; i++)
    {
            _UDPServer->setReceiverPort(_Nreaders[i]);
            _UDPServer->send("dr");
        
            
    }
    _reading = 0;
}


int qrServers::handleDr()
{
    _reading = 0;
}


int qrServers::handlePostnw()
{
    string idx = to_string(qrArtCollect.size()+1);
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string indent = to_string(0);

    for(int i =0; i<_Nw-1; i++)
    {
        if (_Nreaders[i] != _coordPort)
        {
            _UDPServer->setReceiverPort(_Nwriters[i]);
            _UDPServer->send("Update");
            _UDPServer->send(idx);
            _UDPServer->send(tt);
            _UDPServer->send(ct);
            _UDPServer->send(indent);
        }
        
    }
    qrArticle new_a;
    new_a.idx = qrArtCollect.size() + 1;
    new_a.indent = 0;
    new_a.title = tt;
    new_a.content = ct;

    qrArtCollect.push_back(new_a);
    _writing = 0;
    this->sync();
}

int qrServers::handleReplynw()
{
    string idstr = _UDPServer->listen();
    string tt = _UDPServer->listen();
    string ct = _UDPServer->listen();
    string indentstr = _UDPServer->listen();

    for (int i = 0; i < _Nw-1; i++)
    {
        if (_Nreaders[i] != _coordPort)
        {
            _UDPServer->setReceiverPort(_Nwriters[i]);
            _UDPServer->send("Update");
            _UDPServer->send(idstr);
            _UDPServer->send(tt);
            _UDPServer->send(ct);
            _UDPServer->send(indentstr);
        }
    }


    qrArticle new_a;
    stringstream tmp(idstr);
    tmp >> new_a.idx;
    stringstream tmp2(indentstr);
    tmp2 >> new_a.indent;
    new_a.title = tt;
    new_a.content = ct;

    qrArtCollect.push_back(new_a);
    _writing = 0;
    this->sync();
}

int qrServers::handleReqnr()
{
    string nstr = _UDPServer->listen();
    int n = 0;
    stringstream tmp(nstr);
    tmp >> n;
    int nr = generateNr(n);

    _UDPServer->send(to_string(nr));
}

int qrServers::handleReqnw()
{
    string nstr = _UDPServer->listen();
    int n = 0;
    stringstream tmp(nstr);
    tmp >> n;
    int nw = generateNw(n);

    _UDPServer->send(to_string(nw));
}