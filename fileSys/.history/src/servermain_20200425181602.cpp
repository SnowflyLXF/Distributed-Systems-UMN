#include "trackServer.hpp"

using namespace std;

int main()
{
    trackServer S(5050);
    while(1)
    {
        S.OnConnect();
    }
}