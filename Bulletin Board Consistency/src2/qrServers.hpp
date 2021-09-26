#include "server.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex>
#include <vector>

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
    char _directname[14];
    vector<int> _servers;

public:
    qrServers(int, int);
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
};
