#include "header.hpp"

bool haveRgbTable = 0;
const int maxn = 256;
float rgb02568[maxn],rgb05401[maxn],rgb00979[maxn];
float rgb01448[maxn], rgb02913[maxn], rgb04392[maxn];
float rgb03677[maxn], rgb00714[maxn];

void getTable()
{
    for (int i = 0;i < 256;i ++) {
        rgb00714[i] = 0.0714 * i;
		rgb00979[i] = 0.0979 * i;
		rgb01448[i] = 0.1448 * i;
		rgb02568[i] = 0.2568 * i;
		rgb02913[i] = 0.2913 * i;
		rgb03677[i] = 0.3677 * i;
		rgb04392[i] = 0.4392 * i;
		rgb05401[i] = 0.5401 * i;
    }
}

void rgb2yuv(uchar* inBuffer, int width, int height, string& outPath)
{
    if (!haveRgbTable) {
        haveRgbTable = 1;
        getTable();
    }
    ofstream out(outPath, ios :: binary);
    if (!out.is_open()) {
        cout << "Open Failed" << endl;
        exit(0);
    }
    int size = width * height;
    uchar* Y, *U, *V;
    Y = new uchar[size];
    U = new uchar[size / 4];
    V = new uchar[size / 4];
    int num = 0;
    for (int i = 0;i < size;i ++) {
        uchar r, g, b;
        int j = 0;
        for (auto k : {&b, &g, &r}) *k = inBuffer[i * 3 + j ++];
        Y[i] = int(rgb02568[r] + rgb05401[g] + rgb00979[b]) + 16;
        int h = i / width, w = i % width;
        if ((h & 1) || (w & 1)) continue;
        U[num] = int(-rgb01448[r] - rgb02913[g] + rgb04392[b]) + 128;
        V[num ++] = int(rgb04392[r] - rgb03677[g] - rgb00714[b]) + 128;
    }
    out.write((char*)Y, size);
    out.write((char*)U, size / 4);
    out.write((char*)V, size / 4);
    out.close();
    delete[] Y;
    delete[] U;
    delete[] V;
    return ;
}