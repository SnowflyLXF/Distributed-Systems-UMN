#include <trackServer.hpp>

int main()
{
    trackServer S(5050);
    while(1)
    {
        S.OnConnect();
    }
}