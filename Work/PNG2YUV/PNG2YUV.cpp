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

const string path = "Panda80.png";
const string out_path = "out.idat";
const string in_path = "in.idat";
const string out_yuv = "out4.yuv";

map<string, vector<int> > mp;
set <string> ancillary_chunk;
vector<Chunks> v;

struct iHDR
{
    unsigned int width, height;
    unsigned char bit_depth, color_type, compression_method;
    unsigned char filter_method, interlace_method;
    void GetIHDR(unsigned char* buffer)
    {
        for( int i = 0; i < 4; ++ i ) { width = (width << 8) + buffer[i]; }
        for( int i = 0; i < 4; ++ i ) { height = (height << 8) + buffer[i + 4]; }
        int pos = 8;
        for(auto i : {&bit_depth, &color_type, &compression_method, &filter_method, &interlace_method})
            *i = buffer[pos ++];
    }
    void Print()
    {
        cout << width << " " << height << endl;
        for( auto i : {bit_depth, color_type, compression_method, filter_method, interlace_method})
            cout << (int)i << " ";
        cout << endl;
    }
}ihdr;

void TranslateIDAT(ofstream& out, uchar* buffer, int bufferLength)
{
    out.write((char*)buffer, bufferLength);
    return ;
}

void OutputIDAT()
{
    ifstream in(out_path, ios :: binary);
    if (in.is_open()) return ;
    ofstream out(out_path, ios :: binary);
    for (auto &i : mp["IDAT"]) TranslateIDAT(out, v[i].data, v[i].length);
    return ;
}

struct pLTE
{
    int r, g, b;
    static pLTE * GetPLET(uchar* inBuffer) {
        int plte_size = 1 << ihdr.bit_depth;
        pLTE* plte = new pLTE[plte_size];
        int pos = 0;
        for (int i = 0;i < plte_size;i ++) {
            for (auto j : {&plte[i].r, &plte[i].g, &plte[i].b}) {
                *j = inBuffer[pos ++];
            }
        }
        return plte;
    }
    void Print() {
        for (auto i : {r, g, b}) cout << i << ' ';
        cout << endl;
        return ;
    }
}*plte;

void png2yuv(string out_yuv_path, uchar* buffer, uint buffer_length)
{
    unsigned char* y, * u, * v;
    int width = ihdr.width, height = ihdr.height;
    int y_size = width * height;
    int uv_size = y_size / 4;
    cout << y_size << ' ' << uv_size << endl;
    y = new unsigned char[y_size];
    u = new unsigned char[uv_size];
    v = new unsigned char[uv_size];
    int uv_pos = 0, y_pos = 0;
    for( int i = 0; i < buffer_length; ++ i )
    {
        if((i + 1) % (width + 1) == 0) continue;
        int r, g, b; auto plte_tmp = plte[buffer[i]];
        r = plte_tmp.r, g = plte_tmp.g, b = plte_tmp.b;
        int h = i / (width + 1 ), w = i % (width + 1);
        y[y_pos ++] = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
        if((h & 1) || (w & 1)) continue;
        u[uv_pos] = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
        v[uv_pos ++] =  ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
    }
    cout << y_pos << ' ' << uv_pos << endl;
    ofstream out(out_yuv_path, ios::binary);
    out.write((char*)y, y_size); 
    out.write((char*)u, uv_size); out.write((char*)v, uv_size);
    out.close();
    for(auto i : {&y, &u, &v}) delete[] *i;
    cout << "ok" << endl;
    return ;
}

int main()
{
    ifstream in(path, ios :: binary);
    file_header.GetHead(in);
    int pos = -1;
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
    //获取IHDR信息
    ihdr.GetIHDR(v[position].data);
    ihdr.Print();
    //获取IDAT信息
    OutputIDAT();
    position = mp["PLTE"][0];
    plte = pLTE::GetPLET(v[position].data);
    cout << "OK" << endl;
    
    // 解压缩IDAT后进行转换
    in.open(in_path, ios :: binary);
    cout << "Ok" << endl;
    in.seekg(0, ios :: end);
    int length = in.tellg();
    cout << length << endl;
    in.seekg(0, ios :: beg);
    uchar* data_buffer = new uchar[length];
    in.read((char*)data_buffer, length);
    png2yuv(out_yuv, data_buffer, length);
    return 0;
}