#include "pbServers.hpp"
#include "lwServers.hpp"
#include "qrServers.hpp"
using namespace std;

int main()
{
    // pbServers pb1(5050, 1);
    // pbServers pb2(5049, 0);
    // pbServers pb3(5048, 0);

    // lwServers lw1(5050,1);

    qrServers qr1(5050, 1, 1, 1);

    while (1)
    {
        // pb1.handleConnect();
        // lw1.handleConnect();
        qr1.handleConnect();
    }
    // pb1.handleRead();
}