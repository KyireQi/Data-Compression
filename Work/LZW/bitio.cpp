/*
 * Definitions for bitwise IO
 *
 * vim: ts=4 sw=4 cindent
 */

#include <stdlib.h>
#include <stdio.h>
#include "bitio.h"
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
#if 0
int main( int argc, char **argv){
	BITFILE *bfi, *bfo;
	int bit;
	int count = 0;

	if( 1<argc){
		if( NULL==OpenBitFileInput( bfi, argv[1])){
			fprintf( stderr, "fail open the file\n");
			return -1;
		}
	}else{
		if( NULL==OpenBitFileInput( bfi, NULL)){
			fprintf( stderr, "fail open stdin\n");
			return -2;
		}
	}
	if( 2<argc){
		if( NULL==OpenBitFileOutput( bfo, argv[2])){
			fprintf( stderr, "fail open file for output\n");
			return -3;
		}
	}else{
		if( NULL==OpenBitFileOutput( bfo, NULL)){
			fprintf( stderr, "fail open stdout\n");
			return -4;
		}
	}
	while( 1){
		bit = BitInput( bfi);
		fprintf( stderr, "%d", bit);
		count ++;
		if( 0==(count&7))fprintf( stderr, " ");
		BitOutput( bfo, bit);
	}
	return 0;
}
#endif
