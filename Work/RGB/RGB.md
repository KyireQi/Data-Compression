# RGB文件三通道分量的熵的计算

所给的RGB文件是按照b,r,g的顺序排列，文件分辨率$256\times256$
代码实现了以下功能：
- 计算RGB三通道分量的熵
- 将结果导出到.cvs文件中

为了方便理解，用Excel进行了数据可视化。

```cpp
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>
#include <fstream>
#define uchar unsigned char
#define F FILE

using namespace std;

const int weight = 256;
const int height = 256;

char cvs_name[weight * height] = "C:\\Users\\sdlwq\\Desktop\\Github\\Data-Compression\\Work\\RGB\\Answer.cvs"; 
char file_name[weight * height] = "C:\\Users\\sdlwq\\Documents\\WeChat Files\\wxid_nzxxvy7wrx5q22\\FileStorage\\File\\2022-02\\test.rgb";
vector<uchar > R, G, B;
uchar RGB[weight * height * 3];
int r[weight * height], b[weight * height], g[weight * height];
double entR[weight * height], entG[weight * height], entB[weight * height];

int main()
{
    //read file
    F *file = NULL;
    file = fopen (file_name, "rb");
    if (file == NULL) {
        printf ("Failed To Open!\n");
        return 0;
    }
    else printf ("Successfully Opened\n");
    //statistics RGB
    fread (RGB, sizeof(uchar), 3 * weight * height, file);
    //查阅资料知，RGB的存储方式在计算机中表现为BGR
    for (int i = 0;i < 3 * weight * height;i ++) {
        if (i % 3 == 0) B.push_back(RGB[i]);
        else if (i % 3 == 1) G.push_back(RGB[i]);
        else R.push_back(RGB[i]);
    }
    //统计RGB每个取值下的数目
    int sum = weight * height;
    for (int i = 0;i < 256;i ++) {
        for (int j = 0;j < sum;j ++) {
            if (i == R[j]) r[i] ++;
            if (i == B[j]) b[i] ++;
            if (i == G[j]) g[i] ++;
        }
    }
    //求概率
    for (int i = 0;i < 256;i ++) {
        entR[i] = (1.0 * r[i]) / sum;
        entG[i] = (1.0 * g[i]) / sum;
        entB[i] = (1.0 * b[i]) / sum;
    }
    //求熵
    double entropyR = 0.0, entropyG = 0.0, entropyB = 0.0;
    for (int i = 0;i < 256;i ++) {
        if (entR[i]) entropyR -= (entR[i] * log(entR[i]) / log(2));
        if (entG[i]) entropyG -= (entG[i] * log(entG[i]) / log(2));
        if (entB[i]) entropyB -= (entB[i] * log(entB[i]) / log(2));
    }
    cout << "R = " << entropyR << ' ' << "G = " << entropyG << ' ' << "B = " << entropyB << endl; 
    //写入结果到.cvs
    ofstream oFile;
    oFile.open(cvs_name, ios :: out | ios :: trunc);
    oFile << "Value" << "," << "R" << "," << "G" << "," << "B" << endl;
    for (int i = 0;i < 256;i ++)
        oFile << i << "," << entR[i] << "," << entG[i] << "," << entB[i] << endl;
    oFile << "Class" << "," << "Entropy" << endl;
    oFile << "R" << "," << entropyR << endl;
    oFile << "G" << "," << entropyG << endl;
    oFile << "B" << "," << entropyB << endl;
    fclose(file);
    return 0;
}
```
得到的结果：
```
Class	Entropy		
R	7.22955		
G	7.17846		
B	6.85686	
```
使用Excel做出图像：
<img src = "C:\Users\sdlwq\Desktop\Github\Data-Compression\Graph\7.png" width = 70%>

