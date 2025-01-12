/*
 *  O B J F U N . H
 *
 * last modified on Thu May 30 13:04:33 2024, by O.F.H.
 *
 * written by O.Holland
 *
 */


#ifndef OBJFUN_H
#define OBJFUN_H

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

#define INTEL_RECORD_MARK ':'
#define INTEL_DATA_RECORD 0
#define INTEL_END_RECORD 1
#define INTEL_EXT_SEG_ADDR_RECORD 2
#define INTEL_STRT_SEG_ADDR_RECORD 3
#define INTEL_EXT_LIN_ADDR_RECORD 4
#define INTEL_STRT_LIN_ADDR_RECORD 5

#define MOTOROLA_RECORD_MARK 'S'
#define MOTOROLA_COMMENT_RECORD 0
#define MOTOROLA_DATA_16_RECORD 1
#define MOTOROLA_END_16_RECORD 9
#define MOTOROLA_DATA_24_RECORD 2
#define MOTOROLA_END_24_RECORD 8
#define MOTOROLA_DATA_32_RECORD 3
#define MOTOROLA_END_32_RECORD 7
#define MOTOROLA_UNUSED_RECORD 4
#define MOTOROLA_COUNT_16_RECORD 5
#define MOTOROLA_COUNT_24_RECORD 6

struct recordHdrIntel {
	char  startOfRecordMark;	/* ":" */
	char  recordLength[ 2 ];
	char  recordAddress[ 4 ];
	char  recordType[ 2 ];
	char  recordData[ 1 ];
};

/* S11E0000CE7679FFE41DCE3873FFE41FCE4040FFE421CEF0A2FFE4197EF0BBD2 */
/* S9030000FC */
struct recordHdrMotorola16 {
	char  startOfRecordMark;	/* 'S' */
	char  recordType;			/* '9' */
	char  recordLength[ 2 ];	/* "03" */
	char  recordAddress[ 4 ];	/* "0000" */
	char  recordData[ 1 ];
};

/* S804000000FC */
struct recordHdrMotorola24 {
	char  startOfRecordMark;	/* 'S' */
	char  recordType;			/* '8' */
	char  recordLength[ 2 ];	/* "04" */
	char  recordAddress[ 6 ];	/* "000000" */
	char  recordData[ 1 ];
};

/* S70500000000FC */
struct recordHdrMotorola32 {
	char  startOfRecordMark;	/* 'S' */
	char  recordType;			/* '7' */
	char  recordLength[ 2 ];	/* "05" */
	char  recordAddress[ 8 ];	/* "00000000" */
	char  recordData[ 1 ];
};

union recordHdr {
	struct recordHdrIntel  recHdrInt;
	struct recordHdrMotorola16  recHdrMot16;
	struct recordHdrMotorola24  recHdrMot24;
	struct recordHdrMotorola32  recHdrMot32;
};

struct hdrInfo {
	unsigned char  recordLength;
	unsigned char  recordType;
	unsigned short  recordAddress;
	unsigned short  extraAddress;
};

int  get_byte(FILE *  fp, int *  byte );
int  getbyte(FILE *  fp, int *  byte, int *  chksum, int *  recordlen );
int  getword(FILE *  fp, int *  word, int *  chksum );
int  conv( struct config *  cfg, FILE *  fp, FILE *  ofp );

#endif
