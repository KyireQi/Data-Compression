#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#define uchar unsigned char

using namespace std;

const string path = "test.wav";
vector<string> ans;

struct RiffHeader
{
    string id = "";
    string type = "";
    unsigned int length = 0;
    uchar len[4];
    uchar Type[4];
    void GetHead(ifstream & in) {
        uchar* buffer = new uchar[4];
        in.read((char *)buffer, 4);
        for (int i = 0;i < 4;i ++) id += (int)buffer[i];
        in.read((char *)len, 4);
        length = (len[1] + (len[0] << 8)) + ((len[3] + (len[2] << 8)) << 8);
        in.read((char *)Type, 4);
        for (auto i : Type) type += (int)i;
        return ; 
    }
};

struct FormatHeader
{
    string id = "";
    uchar data[20];
    void GetHead(ifstream & in) {
        uchar* buffer = new uchar[4];
        in.read((char*)buffer, 4);
        for (int i = 0;i < 4;i ++) id += (int)buffer[i];
        in.read((char*)data, 20);
        return ;
    }
};

struct DataHeader
{
    string id = "";
    uchar* data;
    unsigned int length = 0;
    void GetHead(ifstream & in) {
        uchar* buffer = new uchar[4];
        in.read((char*)buffer, 4);
        for (int i = 0;i < 4;i ++) id += (int)buffer[i];
        uchar* len = new uchar[4];
        in.read((char*)len, 4);
        length = (len[1] + (len[0] << 8)) + ((len[3] + (len[2] << 8)) << 8);
        data = new uchar[length];
        in.read((char *)data, length);
        return ;
    }
};

int main()
{
    ifstream in(path, ios :: binary);
    RiffHeader riff;
    FormatHeader format;
    riff.GetHead(in);
    ans.push_back(riff.id);
    format.GetHead(in);
    ans.push_back(format.id);
    while (!in.eof()) {
        DataHeader data;
        data.GetHead(in);
        ans.push_back(data.id);
    }
    cout << "All chunks : " << endl;
    for (auto i : ans) cout << "Chunk id : " << i << endl;
    return 0;
}