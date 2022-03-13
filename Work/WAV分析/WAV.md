@[toc]
# WAV格式文件分析
## WAV格式简介
>WAV是最常见的声音文件格式之一，是微软公司专门为Windows开发的一种标准数字音频文件，该文件能记录各种单声道或立体声的声音信息，并能保证声音不失真。它符合资源互换文件格式（RIFF）规范，用于保存Windows平台的音频信息资源，被Windows平台及其应用程序所广泛支持。Wave格式支持MSADPCM、CCITT A律、CCITT μ律和其他压缩算法，支持多种音频位数、采样频率和声道，是PC机上最为流行的声音文件格式；但其文件尺寸较大，多用于存储简短的声音片段。

来源：百度百科

## WAV格式组成
WAV文件**遵循RIFF规则**，其内容以`chunk`为最小单位进行存储。WAV文件一般由三个区块组成：`RIFF chunk`，`Format chunk`和`Data chunk`。同时，文件中也可能存在一些可选的区块，比如：`Fact chunk`，`PlayList chunk`等。在分析的过程中，我们重点分析前三种区块：`RIFF chunk`，`Format chunk`和`Data chunk`。

下面详细给出各区块的组成结构：

### RIFF Chunk
|名称|偏移地址|字节数|端序|内容|
|-|-|-|-|-|
|ID|0x00|4|大端|RIFF (0x52494646)|
|Size|0x04|4|小端|fileSize - 8|
|Type|0x08|4|大端|WAVE(0x57415645)|

- 以`RIFF`为标识
- `Size`是指的整个文件的大小减去`ID`和`Size`的长度。故是$filesize - 8$
- `Type`为`Wave`表示后面需要有两个子块：`Format`和`Data`

### Format Chunk
|名称|偏移地址|字节数|端序|内容|
|-|-|-|-|-|
|ID|0x00|4|大端|fmt (0x666D7420)|
|Size|0x04|4|小端|16/18|
|AudioFormat|0x08|2|小端|音频格式|
|NumChannels|0x0A|2|小端|声道数|
|SampleRate|0x0C|4|小端|采样率|
|ByteRate|0x10|4|小端|每秒数据字节数|
|BlockAlign|0x14|2|小端|数据块对齐|
|BitsPerSample|0x16|2|小端|采样位数|

- 以`fmt`为标识
- `Size`表示该区块数据的长度（不包含ID和Size的长度）为16时`WAV`头部**不包含附加信息**。
- `AudioFormat`表示`Data`区块存储的音频数据的格式，`PCM`音频数据的值为1
- `NumChannels`表示音频数据的声道数，1：单声道，2：双声道
- `SampleRate`表示音频数据的采样率
- `ByteRate`每秒数据字节数 $SampleRate * NumChannels * BitsPerSample / 8$
- `BlockAlign`每个采样所需的字节数 $NumChannels * BitsPerSample / 8$
- `BitsPerSample`每个采样存储的bit数，取值有8，16，32

### Data Chunk

|名称|偏移地址|字节数|端序|内容|
|-|-|-|-|-|
|ID|0x00|4|大端|data(0x64617461)|
|Size|0x04|4|小端|视实际情况而定|
|Data|0x08|视文件大小而定|小端|音频数据|

- `Data`为标识
- `Size`表示音频的长度$ByteRate * Seconds$
- `Data`表示数据

对于`Data Chunk`，声道数和采样率不同，造成不同的数据布局：（每列1Byte大小）

#### 8 bit 单声道
|采样1|采样2|
|-|-|
|数据1|数据2|

#### 8 bit 双声道
|采样1||采样2||
|-|-|-|-|
|声道1 数据1|声道2 数据1|声道1 数据2| 声道2 数据2|

#### 16 bit 单声道
|采样1||采样2||
|-|-|-|-|
|数据1 低字节|数据1 高字节|数据2 低字节|数据2 高字节|

#### 16 bit 双声道
|采样1||||
|-|-|-|-|
|声道1 数据1 低字节|声道1 数据1 高字节|声道2 数据1 低字节|声道2 数据1 高字节|
|采样2||||
|声道1 数据2 低字节|声道1 数据2 高字节|声道2 数据2 低字节|声道2 数据2 高字节|

下面解释一下在上述内容中经常出现的大小端序问题

## 大小端端序

Wave文件以**小端端序**来存储数据
- 大端模式，是指**数据的低位保存在内存的高地址中**，而数据的高位，保存在内存的低地址中，如PNG文件格式；
- 小端模式，是指**数据的低位保存在内存的低地址中**，而数据的高位保存在内存的高地址中。


下面我们分析一个具体的.wav文件：

## 实际文件分析
### RIFF Chunk
|名称|实际数据|说明|
|-|-|-|
|ID|![在这里插入图片描述](https://img-blog.csdnimg.cn/e43519fcc8334e2bb7b74723415c3813.png)|和上述内容一致|
|Size|![在这里插入图片描述](https://img-blog.csdnimg.cn/d59a883a9928467db33466256a53c00d.png)|整个文件大小为45340字节
|Type|![在这里插入图片描述](https://img-blog.csdnimg.cn/d847816058f0400c8c2f2cbb384debe0.png)|文件类型为WAVE|

### Format Chunk
|名称|实际数据|说明|
|-|-|-|
|ID|![在这里插入图片描述](https://img-blog.csdnimg.cn/50ff7480e8474637adc0dd67e2802361.png)|和上文描述一致|
|Size|![在这里插入图片描述](https://img-blog.csdnimg.cn/ec5ea4221f144e23857b37b0d986455a.png)|大小为16，头部不含附加信息|
|AudioFormat|![在这里插入图片描述](https://img-blog.csdnimg.cn/46b0a16cc2274414b296f011f2960d3e.png)|为PCM音频数据|
|NumChannels|![在这里插入图片描述](https://img-blog.csdnimg.cn/c0816c67b8c54af09e3f4616772129af.png)|单声道音频|
|SampleRate|![在这里插入图片描述](https://img-blog.csdnimg.cn/18380c4e9a364bb9bbf81678098bf13f.png)|采样率为22050|
|ByteRate|![在这里插入图片描述](https://img-blog.csdnimg.cn/5c4ccda9bfba40099b66ed9121d7c392.png)|每秒数据字节数为44100|
|BlockAlign|![在这里插入图片描述](https://img-blog.csdnimg.cn/5aa00c58d2a749f499f151e52e788560.png)|每个采样所需字节数为2|
|BitsPerSample|![在这里插入图片描述](https://img-blog.csdnimg.cn/37bcaba4d8af41db90f3c4df41b58366.png)|每个采样存储16bit|

### Data Chunk
|名称|实际数据|说明|
|-|-|-|
|ID|![在这里插入图片描述](https://img-blog.csdnimg.cn/9b9c00cd4c804bc8b033bb190be59653.png)|和上文描述一致|
|Size|![在这里插入图片描述](https://img-blog.csdnimg.cn/4803a87e13144898936c51a81078a28d.png)|数据长度为45304字节|
|Data|好多不放了就....|实际存储的数据|

### 是否存在其他可选区块？

为了验证该文件是否存在可选区块，加入了以下代码：

```cpp
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
```

得到结果：

```
All chunks : 
Chunk id : RIFF
Chunk id : fmt 
Chunk id : data
```
本文件中无可选数据块。

## 参考文献

[WAV文件格式详解](https://blog.csdn.net/imxiangzi/article/details/80265978)
[WAVE文件格式分析](https://www.cnblogs.com/muxue/archive/2010/04/19/1715715.html)