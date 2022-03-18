#include "header.hpp"

void rgb2bmp(uchar* inBuffer, int width, int height, string& outPath)
{
    ofstream out(outPath, ios :: binary);
    if (!out.is_open()) {
        cout << "Open Failed" << endl;
        exit(0);
    }
    bmpHeader header;
    bmpInfomation bmpInfo; 
    uint size = 0;
    header.type = 0x4d42;
    header.size = size = width * height * 3;
    header.offBits = 54;
    bmpInfo.height = height;
    bmpInfo.width = width;
    out.write((char*)&header, sizeof(bmpHeader));
    out.write((char*)&bmpInfo, sizeof(bmpInfomation));
    uchar* outBuffer = new uchar[size];
    for (int i = 0;i < height;i ++) {
        int W = width * 3;
        for (int j = 0;j < 3 * width;j ++) {
            outBuffer[(height - 1 - i) * W + j] = inBuffer[i * W + j];
        }
    }
    out.write((char*)outBuffer, size);
    delete [] outBuffer;
    out.close();
}