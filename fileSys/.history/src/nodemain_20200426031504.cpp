#include "node.hpp"

using namespace std;


int main()
{
    Node n1(5055,5056);
    n1.SetServer("127.0.0.1", 5050);
    n1.Connect();
    n1.FileList();
    n1.Find("ajoy.txt");
    string a = gets();
}