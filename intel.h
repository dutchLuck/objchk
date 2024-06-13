/*
 *  I N T E L . H
 *
 * last modified on Thu May 30 13:04:33 2024, by O.F.H.
 *
 * written by O.Holland
 *
 */


#ifndef INTEL_H
#define INTEL_H

#include <stdio.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#define SP 0x20
#define LAST_PRINTABLE_ASCII 0x7E
#define BYTEMASK 0xFF
#define OKCHECKSUM 0
#define UNLIKELYSTARTADDRESS 0xFFFF
#define REQLENGTH 16
#define RECORD_MARK ':'
#define DATA_RECORD 0
#define END_RECORD 1
#define EXT_SEG_ADDR_RECORD 2
#define STRT_SEG_ADDR_RECORD 3
#define EXT_LIN_ADDR_RECORD 4
#define STRT_LIN_ADDR_RECORD 5

struct recordHdr {
	char  startOfRecordMark;
	char  recordLength[ 2 ];
	char  recordAddress[ 4 ];
	char  recordType[ 2 ];
	char  recordData[ 1 ];
};

struct hdrInfo {
	unsigned char  recordLength;
	unsigned short  recordAddress;
	unsigned char  recordType;
	unsigned short  extraAddress;
};

int  get_byte(FILE *  fp, int *  byte );
int  getbyte(FILE *  fp, int *  byte, int *  chksum, int *  recordlen );
int  getword(FILE *  fp, int *  word, int *  chksum );
int  conv( struct config *  cfg, FILE *  fp, FILE *  ofp );

#endif
