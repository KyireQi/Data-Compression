#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#define uchar unsigned char
#define uint unsigned int

using namespace std;

const string path1 = "out1.yuv";
const string path2 = "out2.yuv";
const string path3 = "out3.yuv";
const string path4 = "out4.yuv";
const string out_yuv_path = "ans.yuv";

int main()
{
    uchar* outputYuv = NULL;
    ifstream in1(path1, ios :: binary);
    ifstream in2(path2, ios :: binary);
    ifstream in3(path3, ios :: binary);
    ifstream in4(path4, ios :: binary);

    int length = 0, len1 = 0, len2 = 0, len3 = 0, len4 = 0;
    in1.seekg(0, ios :: end);
    len1 += in1.tellg();
    length += len1;
    in1.seekg(0, ios :: beg);

    in2.seekg(0, ios :: end);
    len2 += in2.tellg();
    length += len2;
    in2.seekg(0, ios :: beg);

    in3.seekg(0, ios :: end);
    len3 += in3.tellg();
    length += len3;
    in3.seekg(0, ios :: beg);

    in4.seekg(0, ios :: end);
    len4 += in4.tellg();
    length += len4;
    in4.seekg(0, ios :: beg);

    outputYuv = new uchar[length * 50];
    uchar* data_buffer = new uchar[len1];
    in1.read((char*)data_buffer, len1);
    for (int i = 0;i < 50;i ++) {
        for (int j = 0;j < len1;j ++) {
            outputYuv[j + i * len1] = data_buffer[j];
        }
    }
    in2.read((char*)data_buffer, len2);
    for (int i = 0;i < 50;i ++) {
        for (int j = 0;j < len2;j ++) {
            outputYuv[j + (i + 50) * len2] = data_buffer[j];
        }
    }
    in3.read((char*)data_buffer, len3);
    for (int i = 0;i < 50;i ++) {
        for (int j = 0;j < len4;j ++) {
            outputYuv[j + (i + 100) * len3] = data_buffer[j];
        }
    }
    in4.read((char*)data_buffer, len4);
    for (int i = 0;i < 50;i ++) {
        for (int j = 0;j < len3;j ++) {
            outputYuv[j + (i + 150) * len4] = data_buffer[j];
        }
    }
    ofstream out(out_yuv_path, ios :: binary);
    out.write((char*)outputYuv, 50 * length);
    delete [] data_buffer;
    delete [] outputYuv; 
    return 0;
}