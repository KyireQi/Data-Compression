#pragma once

#define uchar unsigned char
#define uint unsigned int

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

void rgb2yuv(uchar* inBuffer, int width, int height, string& outPath);
void yuv2rgb(uchar* inBuffer, int width, int height, string& outPath);
void egb2bmp(uchar* inBuffer, int width, int height, string& outPath);

struct bmpHeader
{
    short type = 0;
    uint size = 0;
    uint reserbed = 0;
    uint offBits = 0;
};

struct bmpInfomation
{
    uint size = 0;
    uint width = 0;
    uint height = 0;
    short planes = 0;
    short bitCount = 0;
    uint compression = 0;
    uint sizeImage = 0;
    uint xPelsPerMeter = 0;
    uint yPelsPerMeter = 0;
    uint clrUsed = 0;
    uint clrImportant = 0;
};