[toc]
# PNG格式文件分析
## PNG格式简介
>便携式网络图形（`Portable Network Graphics`，PNG）是一种采用无损压缩算法的位图格式，其设计目的是试图替代GIF和TIFF文件格式，同时增加一些GIF文件格式所不具备的特性。PNG使用从LZ77派生的无损数据压缩算法，一般应用于JAVA程序、网页或S60程序中，原因是它压缩比高，生成文件体积小。

来源：百度百科

## PNG文件结构如何
PNG文件一般由两大部分组成：

- 文件头：`Filehead`，用来标识这是一个PNG格式的文件。
- 其他部分：`Chunks`，其他的部分则是由多个数据块组成的，存储了描述图像的相关信息。

一个标准的PNG文件结构应该如下：

|FILEHEAD|PNG Chunks|......|PNG Chunks|
|-|-|-|-|



### FILEHEAD的组成
根据PNG文件的定义来说，其文件头位置总是由位固定的字节来描述的：
|十进制表示|十六进制表示|
|-|-|
|137 80 78 71 13 10 26 10|	89 50 4E 47 0D 0A 1A 0A|

实际上，第一个字节的数据`0x89`已经超过了`ASCII`码的范围，**这是为了避免某些软件把PNG文件当作文本文件来处理**。


每一部分的作用如下表：
|取值(HEX)|含义|
|-|-|
|89|表示不支持8bit的数据，并减小一个文本文件被错误地认为是PNG文件的可能性。|
|50 4E 47|英文字符串“PNG”的ASCII码|
|0D 0A|DOS风格的换行符（CRLF）。用于DOS-Unix数据的换行符转换。|
|1A|在DOS命令行下，用于阻止文件显示的文件结束符。|
|0A|Unix风格的换行符（LF）。用于Unix-DOS换行符的转换。|

### PNG Chunk

PNG定义了两种类型的数据块：

- 一种是称为关键数据块(`critical chunk`)，这是标准的数据块。
- 一种叫做辅助数据块(`ancillary chunks`)，这是可选的数据块。

**关键数据块定义了4个标准数据块，每个PNG文件都必须包含它们，PNG读写软件也都必须要支持这些数据块**。虽然PNG文件规范没有要求PNG编译码器对可选数据块进行编码和译码，但规范提倡支持可选数据块。

#### Critical Chunk
关键数据块有四种类型：

- IHDR，`header chunk`，包含有图像基本信息，作为**第一个出现的数据块并且只出现一次**。
- PLTE，`palette chunk`，调色板数据块，**必须存放在图像数据块之前**。
- IDAT，`image data chunk`，存储实际的图像数据。PNG数据包**允许包含多个连续的图像数据块**。
- IEND，`image trailer chunk`，图像结束数据，表示PNG数据流结束。

除上述中的限制外，各类`chunk`的出现顺序没有严格要求。同时在文件中的数据是按照：**高位在前低位在后**的顺寻存储的，这一点和BMP格式并不一致。

#### Ancillary Chunk
辅助数据块的类型众多，PNG文件格式规范制定了10个辅助数据块：

- 背景颜色数据块bKGD(`background color`)。
- 基色和白色度数据块cHRM(`primary chromaticities and white point`)。
- 图像γ数据块gAMA(`image gamma`)。
- 图像直方图数据块hIST(`image histogram`)。
- 物理像素尺寸数据块pHYs(`physical pixel dimensions`)。
- 样本有效位数据块sBIT(`significant bits`)。
- 文本信息数据块tEXt(`textual data`)。
- 图像最后修改时间数据块tIME (`image last-modification time`)。
- 图像透明数据块tRNS (`transparency`)。
- 压缩文本数据块zTXt (`compressed textual data`)。

## 如何获取PNG的元数据信息
在PNG文件中，每个数据块都由下面四部分组成：
|名称|字节数|说明|
|-|-|-|
|Length（长度）|4 bytes|指定数据块中数据域的长度，其长度不超过($2^{31} - 1$)字节|
|Chunk Type Code（数据块类型码）|4 bytes|由`ASCII`码字母组成|
|Chunk Data（数据块数据）|可变长度|存储按照Chunk Type Code指定的数据|
|CRC（循环冗余检测）|4 bytes|检错|

所以，我们每次只需要读取前8个字节，就可以大致判断出数据块的类型和长度。

下面我们具体分析一下PNG各个关键数据块的结构：

### IHDR
文件头数据块IHDR(`header chunk`)：它包含有PNG文件中存储的图像数据的基本信息，并要作为第一个数据块出现在PNG数据流中，而且一个PNG数据流中只能有一个文件头数据块。
文件头数据块**由13字节组成**，它的格式如下表所示。
|名称|字节数|说明|
|-|-|-|
|width|4 bytes|图像宽度，以像素为单位|
|Height|4 bytes|图像高度，以像素为单位|
|Bit depth|1 bytes|图像深度：<br>索引彩色图像：1，2，4或8<br>灰度图像：1，2，4，8或16<br>真彩色图像：8或16|
|ColorType|1 byte|颜色类型：<br>0：灰度图像, 1，2，4，8或16<br>2：真彩色图像，8或16<br>3：索引彩色图像，1，2，4或8<br>4：带α通道数据的灰度图像，8或16<br>6：带α通道数据的真彩色图像，8或16|
|Compression method|1 byte|压缩方法(LZ77派生算法)|
|Filter method|1 byte|滤波器方法|
|Interlace method|1 byte|隔行扫描方法：<br>0：非隔行扫描<br>1： Adam7(由Adam M. Costello开发的7遍隔行扫描方法)|

### PLTE
调色板数据块PLTE(`palette chunk`)包含有与索引彩色图像(`indexed-color image`)相关的彩色变换数据，它仅与索引彩色图像有关，而且要放在图像数据块(`image data chunk`)之前。
**PLTE数据块是定义图像的调色板信息**，PLTE可以包含1~256个调色板信息，每一个调色板信息由**3个字节**组成：

|颜色|字节|意义|
|-|-|-|
|R|1 byte|0 = 黑<br>255 = 红|
|G|1 byte|0 = 黑<br>255 = 绿|
|B|1 byte|0 = 黑<br>255 = 蓝|


因此，**调色板的长度应该是3的倍数**，否则，这将是一个非法的调色板。
对于索引图像，调色板信息是必须的，调色板的颜色索引从0开始编号，然后是1、2……，调色板的颜色数不能超过色深中规定的颜色数（如图像色深为4的时候，调色板中的颜色数不可以超过$2^4=16$），否则，这将导致PNG图像不合法。
真彩色图像和带$\alpha$通道数据的真彩色图像也可以有调色板数据块，目的是便于非真彩色显示程序用它来量化图像数据，从而显示该图像。

### IDAT
图像数据块IDAT(`image data chunk`)：它**存储实际的数据**，在数据流中可包含多个连续顺序的图像数据块。
IDAT存放着图像真正的数据信息，因此，如果能够了解IDAT的结构，我们就可以很方便的生成PNG图像。

### IEND
图像结束数据IEND(`image trailer chunk`)：它用来标记PNG文件或者数据流已经结束，并且**必须要放在文件的尾部。**
十六进制下表示为：
00 00 00 00 49 45 4E 44 AE 42 60 82

## 实例分析

下面通过一张PNG图片为例进行分析：
![](https://img-blog.csdnimg.cn/5c0484b59f4443b2820f29d287368636.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAQ1VDS3lyaWU=,size_20,color_FFFFFF,t_70,g_se,x_16#pic_center)
### 图片中有哪些关键数据块？

##### File header
![在这里插入图片描述](https://img-blog.csdnimg.cn/61acb8c1c034486186198fa660beada0.png)
如前文所述，前8个字节为：89 50 4E 47 0D 0A 1A 0A

##### IHDR

|名称|实际数据|说明|
|-|-|-|
|Length（长度）|![在这里插入图片描述](https://img-blog.csdnimg.cn/dee58ae2d06b44f4a020a85e5f4054a8.png)|数据块长度为13|
|Chunk Type Code|![在这里插入图片描述](https://img-blog.csdnimg.cn/fe6115ac4bab40458809156d74ddb0b4.png)|数据块类型为`IHDR`|
|width|![在这里插入图片描述](https://img-blog.csdnimg.cn/72b4b570f92248ffa26e49ac14297d18.png)|图像宽度为981|
|Height|![在这里插入图片描述](https://img-blog.csdnimg.cn/1b78508f0a6b47eb84ba9ce13cb7f8eb.png)|图像高度为1034|
|Bit depth|![在这里插入图片描述](https://img-blog.csdnimg.cn/b99f5389b61a4f7b99c9e5e98275f09c.png)|图像深度为8位|
|ColorType|![在这里插入图片描述](https://img-blog.csdnimg.cn/48bf8079baf242edaf2e31abd51b8491.png)|颜色类型为3，是索引彩色图像|
|Compression method|![在这里插入图片描述](https://img-blog.csdnimg.cn/9fb4ff00d7d74b55ada2855e671cdeeb.png)|无压缩|
|Filter method|![在这里插入图片描述](https://img-blog.csdnimg.cn/15d278997bbe4a5fadb628159b6c74b0.png)|滤波器方法0|
|Interlace method|![在这里插入图片描述](https://img-blog.csdnimg.cn/59bad26e6f034f4eb056fc37524eabed.png)|0：非隔行扫描|
|CRC|![在这里插入图片描述](https://img-blog.csdnimg.cn/2139663dc01e426b93b886b027c20d06.png)|CRC检错|

##### PLTE
8位深度图，索引彩色图像，所以肯定存在调色板。

|名称|实际结果|说明|
|-|-|-|
|Length|![在这里插入图片描述](https://img-blog.csdnimg.cn/78e9070b6955454fbbefbd6db0664072.png)|数据块长度为768|
|Chunk Type Code|![在这里插入图片描述](https://img-blog.csdnimg.cn/14949a9c6bde4698a2a472870f864af9.png)|数据块类型为PLTE|
|Chunk Data|太多了不放了....|实际数据|

##### IDAT
我们以第一个IDAT块为例：
|名称|实际数据|说明|
|-|-|-|
|Length|![在这里插入图片描述](https://img-blog.csdnimg.cn/0df3af12565848cab7b0efe33f672d69.png)|数据块长度为160695|
|Chunk Type Code|![在这里插入图片描述](https://img-blog.csdnimg.cn/b51cc6f6ff4c4fff8bb7901ba3404049.png)|数据块类型为IDAT|
|Chunk data|太多了也不放了...|数据块中的实际数据|

##### IEND

![在这里插入图片描述](https://img-blog.csdnimg.cn/b12f970f3061403bb5667fee221405fe.png)
如图所示，和上面的描述是一致的。

#### 图片中是否存在辅助数据块？

我们可以通过下面的代码来找出辅助数据块：
```cpp
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <set>
#define uchar unsigned char

using namespace std;

const string path = "test.png";
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
        cout << "Chunk Type :" << i.first  << endl;
    }
    cout << "Ancillary Chunks" << endl;
    for (auto &i : ancillary_chunk) cout << i << endl;
}
```
最后的结果如下：
```
All Chunks
Chunk Type :IDAT
Chunk Type :IEND
Chunk Type :IHDR
Chunk Type :PLTE
Chunk Type :tRNS
Ancillary Chunks
tRNS
```

有一个辅助数据块`tRNS`，是图像透明数据块。
可以在二进制数据中找到他的位置和数据内容：
![在这里插入图片描述](https://img-blog.csdnimg.cn/e58cb81993034fe7bfa25718365c114d.png)
## 总结
为了便于**数据的交换、管理、编辑和呈现**，PNG文件使用了数据块这种形式。

数据块的结构简单，并且可以很方便的对每块信息进行修改、存储、编辑；同时为了方便显示，只要求解码器可以识别关键数据块，这对兼容性的实现有莫大帮助，即以前的版本的解码器依旧可以打开新版本的图片，只是损失部分新的特征，而核心内容的显示是完全没问题的；同时可以非常方便的对文件进行拓展，甚至可以自己定义一些信息，一些对图像的操作、存储一些其他数据——只要自己再编写出相应的解码器即可。

## 参考文献
[1] [PNG文件分析](https://blog.csdn.net/Cross_Entropy/article/details/105179408)
[2] [W3C.Portable Network Graphics (PNG) Specification (Second Edition)](https://www.w3.org/TR/PNG/index-noobject.html#11transinfo)