#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#define uchar unsigned char

using namespace std;

const string path = "Panda.png";
map<string, vector<int> > mp;
set <string> ancillary_chunk;

struct FileHeader
{
    uchar head[8];
    void GetHead(ifstream &in) {
        in.read((char *)head, 8);
    }
    void Print() {
        for (auto i : head) cout << (int) i << ' ';
        cout << endl;
    }
}file_header;

struct Chunks
{
    unsigned int length = 0;
    uchar type[4];
    string c_type = "";
    uchar *data;
    uchar CRC[4];
    void GetChunk(ifstream & in) {
        uchar * buffer = new uchar[4];
        in.read((char *)buffer, 4);
        for (int i = 0;i < 4;i ++) 
            length = (length << 8) + buffer[i];
        in.read((char *)type, 4);
        if (length) {
            data = new uchar[length];
            in.read((char*)data, length);
        }
        in.read((char *)CRC, 4);
        for (auto i : type) c_type += (int)i;
        return ;
    }
}chunk;

int main()
{
    ifstream in(path, ios :: binary);
    if (!in) {
        cout << "Open Failed" << endl;
        exit(0);
    } 
    file_header.GetHead(in);
    int pos = 0;
    while (true) {
        Chunks tmp;
        tmp.GetChunk(in);
        mp[tmp.c_type].push_back(pos);
        pos ++;
        if (tmp.c_type == "IEND") break;
        if (tmp.c_type != "IDAT" && tmp.c_type != "IHDR" && tmp.c_type != "PLTE") ancillary_chunk.insert(tmp.c_type);
        delete(tmp.data);
    }
    cout << "All Chunks" << endl;
    for (auto &i : mp) {
        cout << "Chunk Type :" << i.first << " Chunk Position : ";
        auto v = i.second;
        for (auto j : v) cout << j << ' ';
        cout << endl;
    }
    cout << "Ancillary Chunks" << endl;
    for (auto &i : ancillary_chunk) cout << i << endl;
}