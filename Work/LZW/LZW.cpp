/*
 * Definition for LZW coding 
 *
 * vim: ts=4 sw=4 cindent nowrap
 */
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <cstring>
#include <string.h>
#define MAX_CODE 65535

using namespace std;

typedef struct{
	FILE *fp;
	unsigned char mask;
	int rack;
}BITFILE;

BITFILE *OpenBitFileInput( char *filename){
	BITFILE *bf;
	bf = (BITFILE *)malloc( sizeof(BITFILE));
	if( NULL == bf) return NULL;
	if( NULL == filename)	bf->fp = stdin;
	else bf->fp = fopen( filename, "rb");
	if( NULL == bf->fp) return NULL;
	bf->mask = 0x80;
	bf->rack = 0;
	return bf;
}

BITFILE *OpenBitFileOutput( char *filename){
	BITFILE *bf;
	bf = (BITFILE *)malloc( sizeof(BITFILE));
	if( NULL == bf) return NULL;
	if( NULL == filename)	bf->fp = stdout;
	else bf->fp = fopen( filename, "wb");
	if( NULL == bf->fp) return NULL;
	bf->mask = 0x80;
	bf->rack = 0;
	return bf;
}

void CloseBitFileInput( BITFILE *bf){
	fclose( bf->fp);
	free( bf);
}

void CloseBitFileOutput( BITFILE *bf){
	// Output the remaining bits
	if( 0x80 != bf->mask) fputc( bf->rack, bf->fp);
	fclose( bf->fp);
	free( bf);
}

int BitInput( BITFILE *bf){
	int value;
	if( 0x80 == bf->mask){
		bf->rack = fgetc( bf->fp);
		if( EOF == bf->rack){
			fprintf(stderr, "Read after the end of file reached\n");
			exit( -1);
		}
	}
	value = bf->mask & bf->rack;
	bf->mask >>= 1;
	if( 0==bf->mask) bf->mask = 0x80;
	return( (0==value)?0:1);
}

unsigned long BitsInput( BITFILE *bf, int count){
	unsigned long mask;
	unsigned long value;
	mask = 1L << (count-1);
	value = 0L;
	while( 0!=mask){
		if( 1 == BitInput( bf))
			value |= mask;
		mask >>= 1;
	}
	return value;
}

void BitOutput( BITFILE *bf, int bit){
	if( 0 != bit) bf->rack |= bf->mask;
	bf->mask >>= 1;
	if( 0 == bf->mask){	// eight bits in rack
		fputc( bf->rack, bf->fp);
		bf->rack = 0;
		bf->mask = 0x80;
	}
}

void BitsOutput( BITFILE *bf, unsigned long code, int count){
	unsigned long mask;

	mask = 1L << (count-1);
	while( 0 != mask){
		BitOutput( bf, (int)(0==(code&mask)?0:1));
		mask >>= 1;
	}
}

struct {
	int suffix;
	int parent, firstchild, nextsibling;
} dictionary[MAX_CODE+1];
int next_code;
int d_stack[MAX_CODE]; // stack for decoding a phrase

#define input(f) ((int)BitsInput( f, 16))
#define output(f, x) BitsOutput( f, (unsigned long)(x), 16)

int DecodeString( int start, int code);
void InitDictionary( void);
void PrintDictionary( void){
	int n;
	int count;                                                                                                                                                                                                                                                                                                                 
	for( n=256; n<next_code; n++){
		count = DecodeString( 0, n);
		printf( "%4d->", n);
		while( 0<count--) printf("%c", (char)(d_stack[count]));
		printf( "\n");
	}
}

int DecodeString( int start, int code){  // 从子节点回退父亲节点
	int count;
	count = start; 
	while (0 <= code) {
		d_stack[count] = dictionary[code].suffix;
		code = dictionary[code].parent; // 回退到父节点节点
		count++;
	}
	return count;
}

void InitDictionary(void){
	int i;
	for( i=0; i<256; i++){   //将ASCII中的字符加入到字典中
		dictionary[i].suffix = i;   // 初始化单个字符
		dictionary[i].parent = -1;  // 对于单个字符而言，他们的父节点是空（-1）
		dictionary[i].firstchild = -1; // 对于一开始初始化的节点，他们的孩子节点也是空的
		dictionary[i].nextsibling = i+1;  // 但是所有的单个字符都是属于一个父亲节点
	}
	dictionary[255].nextsibling = -1;  // 最后一个节点没有兄弟
	next_code = 256;  // 下一个加入词典的节点序号为next_code
}
/*
 * Input: string represented by string_code in dictionary,
 * Output: the index of character+string in the dictionary
 * 		index = -1 if not found
 */
int InDictionary( int character, int string_code){  // character是当前字符，string_code是他的父节点值
	int sibling;
	if( 0>string_code) return character;  // 如果此时是单个字符，直接返回
	sibling = dictionary[string_code].firstchild;  // 否则获取父节点的第一个孩子节点
	while( -1 < sibling){  // 查找这颗子树
		if( character == dictionary[sibling].suffix) return sibling;  // 如果找到了，返回节点编号
		sibling = dictionary[sibling].nextsibling; // 查找兄弟节点
	}
	return -1;  // 没有查找到，返回-1
}

void AddToDictionary( int character, int string_code){
	int firstsibling, nextsibling;
	if(0 > string_code) return;
    // 构建出新的节点和父节点之间的关系
	dictionary[next_code].suffix = character;
	dictionary[next_code].parent = string_code;
	dictionary[next_code].nextsibling = -1;
	dictionary[next_code].firstchild = -1;
    // 考虑到他可能存在兄弟，所以需要更新一次兄弟节点
	firstsibling = dictionary[string_code].firstchild;
	if( -1<firstsibling){	// the parent has child
		nextsibling = firstsibling;
		while( -1<dictionary[nextsibling].nextsibling ) // 找出边缘的孩子节点
			nextsibling = dictionary[nextsibling].nextsibling;
		dictionary[nextsibling].nextsibling = next_code;
	}else{// no child before, modify it to be the first
		dictionary[string_code].firstchild = next_code;
	}
	next_code ++; 
}

void LZWEncode(FILE *fp, BITFILE *bf){
	int character;
	int string_code;
	int index;
	unsigned long file_length;

	fseek( fp, 0, SEEK_END);
	file_length = ftell(fp);
	fseek( fp, 0, SEEK_SET);
	BitsOutput( bf, file_length, 4*8);
	InitDictionary();
	string_code = -1;
	while(EOF!=(character=fgetc(fp))){  // 一次读取一个字符
		index = InDictionary(character, string_code); // 判断此时的字串是否在字典中
		if( 0<=index){	// string+character in dictionary
			string_code = index;  // 前缀更新
		}else{	// string+character not in dictionary
			output( bf, string_code);
			if( MAX_CODE > next_code){	// free space in dictionary
				// add string+character to dictionary
				AddToDictionary( character, string_code);
			}
			string_code = character;  // 更新前缀为当前读入的字符
		}
	}
	output(bf, string_code);  // 最后一定要输出一次！
}

void LZWDecode(BITFILE *bf, FILE *fp)  // 这里注意！解码是从子节点开始倒推父节点的！
{
    int character;
    int new_code, last_code;
    int phrase_length;
    unsigned long file_length;
    file_length = BitsInput(bf, 4*8);
    if( -1 == file_length) file_length = 0;
    InitDictionary();
    last_code = -1;
    while(0<file_length){
        new_code = input(bf); // 读入一个新的值
        if(new_code >= next_code){ // 不存在这个字串
            d_stack[0] = character;  // 此时需要用前一个字串的第一个字符
            phrase_length = DecodeString(1, last_code);  // 此时d_stack[0]已经占用了，所以需要从1开始解码前一个字串
        }
        else{
            phrase_length = DecodeString(0, new_code);  // 获得这个字串
        }
        character = d_stack[phrase_length-1];
        while( 0 < phrase_length){
            phrase_length --;
            fputc(d_stack[phrase_length], fp);  // 从后往前取，其实就是从子节点回退到父节点
            file_length--;
        }
        if( MAX_CODE>next_code){// add the new phrase to dictionary
            AddToDictionary(character, last_code);
        }
        last_code = new_code;
    }
}

char path_in[9] = "test.yuv";
char path[12] = "out_yuv.dat";
char path_out[10] = "after.yuv";

int main(int argc, char **argv){
	FILE *fp;
	BITFILE *bf;
	fp = fopen(path_in, "rb");
	bf = OpenBitFileOutput(path);
	if( NULL!=fp && NULL!=bf){
		LZWEncode(fp, bf);
		fclose(fp);
		CloseBitFileOutput(bf);
		fprintf(stdout, "encoding done\n");
	}

	bf = OpenBitFileInput(path);
	fp = fopen(path_out ,"wb");
	if( NULL!=fp && NULL!=bf){
		LZWDecode(bf, fp);
		fclose(fp);
		CloseBitFileInput(bf);
		fprintf(stdout, "decoding done\n");
	}
	return 0;
}
