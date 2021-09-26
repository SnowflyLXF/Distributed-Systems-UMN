#include "node.hpp"

using namespace std;


int main()
{
    Node n1(5000,5001);
    n1.SetServer("127.0.0.1", 4999);
    n1.Connect();
    n1.FileList();

    Node n2(6000,6001);
    n2.SetServer("127.0.0.1", 4999);
    n2.Connect();
    n2.FileList();

    Node n3(7000,7001);
    n3.SetServer("127.0.0.1", 4999);
    n3.Connect();
    n3.FileList();

    ifstream f;
    f.open("node6000/cnm.txt", ios_base::in);
    if(f.is_open())
        cout << "checksum:" << checksum(f) << endl;
    f.close();
    // n1.Find("cnm.txt");
    // n2.handleUpload("127.0.0.1", 5001, "cnm.txt");
    n1.Download("cnm.txt");
    cin.get();
}
