#include "server.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex>
#include <vector>
#include <algorithm>

using namespace std;

typedef struct qrArticle
{
    int idx;
    string title;
    string content;
    int indent;
} qrArticle;

class qrServers
{
private:
    UDPServer *_UDPServer;
    int _isCoordinator, _uniqID, _coordPort, _Port;
    int _Nr, _Nw;
    int _reading, _writing;
    char _directname[14];
    vector<int> _servers;
    vector<int> _Nreaders;
    vector<int> _Nwriters;

public:
    qrServers(int, int, int, int);
    int handleConnect();
    int handlePost();
    int handleRead();
    int handleChoose();
    int handleReply();
    // int handlePPost();
    int update();
    int handleUpdate();
    // int handlePReply();
    int handleIDRequest();
    int handleSync();
    int sync();
    int generateNr(int);
    int generateNw(int);
    int handleReqnr();
    int handleReqnw();
    int handleVoter();
    int handleVotew();
    int handleDoner(int);
    // int handleDonew(int);
    int handleDr();
    // int handleDw();
    int handlePostnw();
    int handleReplynw();
};
