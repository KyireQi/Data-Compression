/*
 * Declaration for bitwise IO
 *
 * vim: ts=4 sw=4 cindent
 */
#ifndef __BITIO__
#define __BITIO__

#include <stdio.h>

typedef struct{
	FILE *fp;
	unsigned char mask;
	int rack;
}BITFILE;

BITFILE *OpenBitFileInput( char *filename);
BITFILE *OpenBitFileOutput( char *filename);
void CloseBitFileInput( BITFILE *bf);
void CloseBitFileOutput( BITFILE *bf);
int BitInput( BITFILE *bf);
unsigned long BitsInput( BITFILE *bf, int count);
void BitOutput( BITFILE *bf, int bit);
void BitsOutput( BITFILE *bf, unsigned long code, int count);
#endif	// __BITIO__
