#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#define uchar unsigned char
#define uint unsigned int

using namespace std;

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
    char type[4];
    string c_type = "";
    unsigned char *data;
    unsigned char CRC[4];
    void GetChunk( ifstream &in )
    {
        unsigned char* buffer = new unsigned char[4];
        in.read((char*)buffer,4);
        for( int i = 0; i < 4; ++ i ) { length = (length << 8) + buffer[i]; }
        in.read(type,4);
        if( length != 0 ) {
            data = new unsigned char[length];
            in.read((char*)data, length);
        }
        in.read((char*)CRC,4);
        for( auto i : type ) c_type += (int)i;
        return;
    }
};

const string path = "1.png";
map<string, vector<int> > mp;
set <string> ancillary_chunk;

struct iHDR
{
    unsigned int width, height;
    unsigned char bit_depth, color_type, compression_method;
    unsigned char filter_method, interlace_method;
    void GetIHDR(unsigned char* buffer )
    {
        for( int i = 0; i < 4; ++ i ) { width = (width << 8) + buffer[i]; }
        for( int i = 0; i < 4; ++ i ) { height = (height << 8) + buffer[i+4]; }
        int pos = 8;
        for(auto i : {&bit_depth, &color_type, &compression_method, &filter_method, &interlace_method})
            *i = buffer[pos ++];
    }
    void Print()
    {
        cout << width << " " << height << endl;
        for( auto i : {bit_depth, color_type, compression_method, filter_method, interlace_method})
            cout << (int)i << " ";
    }
}ihdr;

int main()
{
    ifstream in(path, ios :: binary);
    file_header.GetHead(in);
    int pos = -1;
    vector<Chunks> v;
    while (true) {
        Chunks tmp;
        tmp.GetChunk(in);
        v.push_back(tmp); pos ++;
        mp[tmp.c_type].push_back(pos);
        if (tmp.c_type == "IEND") break;
        if (tmp.c_type != "IDAT" && tmp.c_type != "IHDR" && tmp.c_type != "PLTE") ancillary_chunk.insert(tmp.c_type);
    }
    in.close();
    cout << "All Chunks" << endl;
    for (auto &i : mp) {
        cout << "Chunk Type :" << i.first << " Chunk Position : ";
        auto &v = i.second;
        for (auto &j : v) cout << j << ' ';
        cout << endl;
    }
    cout << "Ancillary Chunks" << endl;
    for (auto &i : ancillary_chunk) cout << i << endl;
    int position = mp["IHDR"][0];
    ihdr.GetIHDR(v[position].data);
    // cout << (uchar)vecChunks[position].data << endl;
    ihdr.Print();
    return 0;
}