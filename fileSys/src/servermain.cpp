#include "trackServer.hpp"

using namespace std;

int main()
{
    trackServer S(4999);
    while(1)
    {
        S.OnConnect();
    }
}