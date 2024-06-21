/*
 *  I N T E L . C
 *
 * last modified on Tue Jun 11 21:16:50 2024, by O.F.H.
 *
 * Code to neatly list an Intel Hex format file
 * and check the checksums while doing the listing.
 *
 * written by O.Holland
 *
 */

#include <stdio.h>
#include <stdlib.h>	/* exit() */
#include <ctype.h>	/* isspace() iscntrl() isalnum() */
#include <string.h>	/* strlcpy() */
#include "genFun.h"	/* convertHexChrNibbleToInt() */
#include "config.h"	/* */
#include "intel.h"	/* RECORD_MARK */

#define  BFFR_SZ  640		/* Input line storage size must fit max record length of 0xff (i.e. data record size of 510 + 11) */
#define  D_BFR_SZ  1024		/* Data storage size must be a power of two (i.e. 2^10) so mask works. 1024 fits two max size records */
#define  D_BFR_MSK  ((D_BFR_SZ)-1)


void  printRecordType( FILE *  ofp, int  recordType ) {
	switch ( recordType ) {
		case DATA_RECORD : fprintf( ofp, "Data" ); break;
		case END_RECORD : fprintf( ofp, "End Of File" ); break;
		case EXT_SEG_ADDR_RECORD : fprintf( ofp, "Extended Segment Address" ); break;
		case STRT_SEG_ADDR_RECORD : fprintf( ofp, "Start Segment Address" ); break;
		case EXT_LIN_ADDR_RECORD : fprintf( ofp, "Extended Linear Address" ); break;
		case STRT_LIN_ADDR_RECORD : fprintf( ofp, "Start Linear Address" ); break;
		default : fprintf( ofp, "Unknown" ); break;
	}
}


void  printNewRecordTypeAddress( struct config *  cfg, FILE *  ofp, int  address, int  recordType ) {
	fprintf( ofp, "<%04x>", address );
	if ( cfg->A.active ) fprintf( ofp, "\n");
	else  {
		fprintf( ofp, " ");
		printRecordType( ofp, recordType );
		fprintf( ofp, " Record\n");
	}
}


/* The following Circular (Ring) buffer is expedient version that sacrifices 1 data */
/* storage location to allow an easy buffer full indication, even though when "full" */
/* the buffer is actually 1 location short of completely full. The buffer must be a */
/* power of to in size for the mask to work. */

int ringBffrFull( int  inIndx, int  outIndx ) {
	return( inIndx == outIndx );
}


int ringBffrEmpty( int  inIndx, int  outIndx, int  ringMask ) {
	return((( outIndx + 1 ) & ringMask ) == inIndx );
}


int  loadByteRingBuffer( struct config *  cfg, char *  lnPtr, int  dtLen,
	unsigned char  ringBffr[], int *  inIndx, int *  outIndx, int  circMask ) {
	int  i;

	for ( i = 0; (! ringBffrFull( *inIndx, *outIndx )) && ( i < dtLen ); i++) {
		ringBffr[ *inIndx ] = ( unsigned char )( convert2HexChrNibblesToInt( lnPtr + i * 2 ) & 0xff );
		*inIndx = ( *inIndx + 1 ) & circMask;
	}
	return( i );	/* return number of bytes loaded */
}


void  dumpRingBuffer( unsigned char  data[] )  {
	for (size_t i = 0; i < D_BFR_SZ; i++)  {
		if (( i % 32 ) == 0 )  fprintf( stdout, "\n%02x", data[ i ] );
		else  fprintf( stdout, " %02x", data[ i ]);
	}
	fprintf( stdout, "\n\n" );
}


int  calcCheckSum( unsigned char  ringBffr[], int  inIndex, int  dataLen, int  circMask )  {
	int  sum = 0;

	for( ; dataLen > 0; dataLen-- )  {
		inIndex = ( inIndex - 1 ) & circMask;
		sum += ringBffr[ inIndex ];
		/* printf( "%d %d 0x%0x, 0x%0x\n", inIndex, dataLen, ringBffr[ inIndex ], sum ); */
	}
	return( sum );
}


void  zeroRingBuffer( unsigned char  data[] )  {
	for (size_t i = 0; i < D_BFR_SZ; i++ )
		data[ i ] = ( unsigned char ) 0;
}


int  ringBffrUsed( int  inIndex, int  outIndex, int  ringMask )  {
	int  diff;

	if( inIndex == outIndex )  return( ringMask );	/* if equal then circular buffer is deemed to be full */
	else if(( diff = ( inIndex - outIndex )) > 0 )  return( diff - 1 );
	else return( diff + ringMask );
}


void  printDataAsHex( struct config *  cfg, FILE *  ofp, unsigned char  dataBfr[],
	int  outIndx, int  circMsk, int  dataCnt ) {
	for( outIndx = (( outIndx + 1 ) & circMsk ); dataCnt-- > 0; outIndx = (( outIndx + 1 ) & circMsk )) {
		fprintf( ofp, "%02x ", dataBfr[ outIndx ] );
	}
	if( dataCnt == 0 ) fprintf( ofp, "%02x", dataBfr[ outIndx ] );
}


void  printDataAsChr( struct config *  cfg, FILE *  ofp, unsigned char  dataBfr[],
	int  outIndx, int  circMsk, int  dataCnt ) {
	int  chr;

	for( outIndx = (( outIndx + 1 ) & circMsk ); dataCnt-- > 0; outIndx = (( outIndx + 1 ) & circMsk )) {
		chr = dataBfr[ outIndx ];
		if(( chr >= (cfg->s.active ? ' ' : '!')) && ( chr <= '~' ))  fprintf( ofp, "%c", chr );
		else  fprintf( ofp, ".");
	}
}


size_t  getA_RecordFromInputSourceIntoStrBfr( FILE *  fp, char *  chrBfr, int  chrBfrLen ) {
	size_t  numOfChrInBfr = 0;
	int  chr;

	if( --chrBfrLen < 9 )	/* pre-decrement to account for string terminator and there are 9 char in the record header */
		fprintf( stderr, "Error: insufficient storage to hold even the shortest record\n" );
	else {
		/* Look for start of record indicator while monitoring for possible EOF */
		while((( chr = fgetc( fp )) != EOF ) && ( chr != RECORD_MARK )) ;
		if(( chr == RECORD_MARK ) && ( numOfChrInBfr < chrBfrLen )) {	/* EOF not found, but start of record marker was just found */
			*chrBfr++ = chr;	/* store record marker character */
			numOfChrInBfr = 1;
			/* get characters while monitoring for possible EOF */
			while(( numOfChrInBfr < chrBfrLen ) && (( chr = fgetc( fp )) != EOF ) && ( chr != '\n' ) && ( chr != '\r' )) {
				if( isalnum( chr )) {	/* Only store alphabetic / numeric characters */
					*chrBfr++ = chr;	/* store character */
					numOfChrInBfr += 1;
				}
			}
			*chrBfr = '\0';		/* terminate string */
		}
	}	/* end of if enough storage space available */
	return( numOfChrInBfr );
}


void  printData( struct config *  cfg, FILE *  ofp, unsigned char  dataBfr[],
	int  outIndx, int  ringBfrMsk, int  dataCnt, int  address ) {
	if( ! cfg->a.active )  fprintf( ofp, "%04x ", address & 0xffff );
	printDataAsHex( cfg, ofp, dataBfr, outIndx, ringBfrMsk, dataCnt );	/* output a Hex data */
	if( ! cfg->A.active )
		printDataAsChr( cfg, ofp, dataBfr, outIndx, ringBfrMsk, dataCnt );	/* output a Char data */
	fprintf( ofp, "\n" );
}


/* conv - returns a negetive number if an error occurs */
/* otherwise it returns the number of characters processed */

int  conv( struct config *  cfg, FILE *  fp, FILE *  ofp ) {
	size_t  bytesProcessed = 0;
	int  errorIndicator = 1;			/* Assume no error at the start */
	int  requiredDataBytesPerLine = REQLENGTH;
	int  dataLen = 0;
	int  dataStart = 0;
	int  recType = 0;
	int  checkSum = 0;
	int  used = 0;
	int  tmp = 0;
	size_t  lineLen = 0;				/* length of Record line in buffer[] */
	char *  linePtr;					/* pointer to start of Record line in buffer[] */
	struct recordHdr *  recHdr;			/* template over Record header in Record line to make extraction easier */
	struct hdrInfo  hdr;				/* header information from current Record */
	char  buffer[ BFFR_SZ ];			/* storage for Record lines from input source */
	unsigned char  data[ D_BFR_SZ ];	/* storage for ring buffer */
	int  inIndex = 0;					/* input index for ring buffer */
	int  outIndex = D_BFR_SZ - 1;		/* output index for ring buffer */

	requiredDataBytesPerLine = ( cfg->w.active) ? cfg->w.optionInt : REQLENGTH;
	zeroRingBuffer( data );
	while (( lineLen = getA_RecordFromInputSourceIntoStrBfr( fp, buffer, BFFR_SZ )) > 0 ) {	/* get line of characters from input source */
		if( cfg->D.active )  fprintf( ofp, "\nDebug: %lu character line received\n", lineLen );
		linePtr = buffer;
		/* Look for start of record marker in string */
		if(( linePtr = strchr( linePtr, RECORD_MARK )) != NULL ) {
			if(( lineLen = strlen( linePtr )) < 9  ) {		/* Don't procede if header is incomplete */
				fprintf( stderr, "Error: Unable to read record header information - input line \"%s\" is too short\n", linePtr );
				errorIndicator = -1;
			}
			else if( strspn( linePtr, ":0123456789ABCDEFabcdef" ) < lineLen ) {		/* Don't procede if record is corrupt */
				fprintf( stderr, "Error: Record \"%s\" has one or more unexpected characters (i.e. not hex or :)\n", linePtr );
				errorIndicator = -1;
			}
			else {
				if( cfg->D.active )  fprintf( ofp, "Debug: %zu character string is: \"%s\"\n", lineLen, linePtr );
				recHdr = ( struct recordHdr * ) linePtr;
				/* check for discontinuity in address between previous record end and new record start */
				if( ! ringBffrEmpty( inIndex, outIndex, D_BFR_MSK )) {
					used = ringBffrUsed( inIndex, outIndex, D_BFR_MSK );
					tmp = convert4HexChrNibblesToInt( recHdr->recordAddress );
					if( cfg->D.active )  fprintf( ofp,
						"Debug: \"dataStart\" is 0x%04x, \"used\" is %d and record Address is 0x%04x\n", dataStart, used, tmp );
					if(( dataStart + used ) != tmp ) {
						printData( cfg, ofp, data, outIndex, D_BFR_MSK, used, dataStart );	/* output nicely formatted data */
						outIndex = ( outIndex + used ) & D_BFR_MSK;
					}
				}
				dataLen = convert2HexChrNibblesToInt( recHdr->recordLength );
				hdr.recordLength = ( unsigned char )( dataLen & 0xff );
				if( cfg->D.active )  fprintf( ofp, "Debug: Data Length 0x%02x ( %d ) bytes.\n", dataLen, dataLen );
				if( lineLen < ( 2 * dataLen + 11 ))	{	/* 9 header nibbles + 2 checksum nibbles + data */
					fprintf( stderr, "Error: Input line \"%s\" is too short (%zu) - needs %d characters\n",
						linePtr, lineLen, ( 2 * dataLen + 11 ));
					errorIndicator = -1;
				}
				else {
					hdr.recordAddress = ( unsigned short )( convert4HexChrNibblesToInt( recHdr->recordAddress ) & 0xffff );
					if( ringBffrEmpty( inIndex, outIndex, D_BFR_MSK ))  dataStart = hdr.recordAddress;		/* intialize the data start */
					if( cfg->D.active )  fprintf( ofp, "Debug: Record Address 0x%04x, ", hdr.recordAddress );
					recType = convert2HexChrNibblesToInt( recHdr->recordType );
					hdr.recordType = ( unsigned char )( recType & 0xff );
					checkSum = ( dataLen + ( hdr.recordAddress >> 8 ) + hdr.recordAddress + recType ) & 0xff;	/* check sum includes length, address and type */
					if( cfg->v.active )  {
						fprintf( ofp, "Info: Record Type %d (", hdr.recordType );
						printRecordType( ofp, recType );
						fprintf( ofp, "), %d data bytes and starting address 0x%04x\n", hdr.recordLength, hdr.recordAddress );
					}
					if(( recType == DATA_RECORD ) && ( dataLen > 0 ))  {
						if( loadByteRingBuffer( cfg, recHdr->recordData, dataLen, data, &inIndex, &outIndex, D_BFR_MSK ) < dataLen ) {
							fprintf( stderr, "Error: Unable to completely load circular buffer\n" );
							errorIndicator = -1;
						}
						checkSum += calcCheckSum( data, inIndex, dataLen, D_BFR_MSK );
					}
					if((( checkSum += convert2HexChrNibblesToInt( recHdr->recordData + ( 2 * dataLen ))) & 0xff ) != 0 ) {
						fprintf( stderr, "Error: Input line \"%s\" has checksum (0x%02x) failure\n", linePtr, checkSum & 0xff );
						errorIndicator = -1;
					}
				}
			}
			bytesProcessed += lineLen;
		}
		if( cfg->D.active ) {
			fprintf( ofp, "Debug: %d data storage locations used\n", ringBffrUsed( inIndex, outIndex, D_BFR_MSK ));
#ifdef DEBUG
			dumpRingBuffer( data );
#endif
		}
		/* Output lines of data of the required width until there is less than a full width line left in the buffer */
		while( ringBffrUsed( inIndex, outIndex, D_BFR_MSK ) >= requiredDataBytesPerLine ) {
			printData( cfg, ofp, data, outIndex, D_BFR_MSK, requiredDataBytesPerLine, dataStart );	/* output nicely formatted data */
			outIndex = ( outIndex + requiredDataBytesPerLine ) & D_BFR_MSK;
			dataStart += requiredDataBytesPerLine;
		}
	}
	if ( ! feof( fp ))  perror( "Error when reading input file" );
	/* Clean up by outputting any data left in the data buffer */
	if(( requiredDataBytesPerLine = ringBffrUsed( inIndex, outIndex, D_BFR_MSK )) > 0 )
		printData( cfg, ofp, data, outIndex, D_BFR_MSK, requiredDataBytesPerLine, dataStart );	/* output nicely formatted data */
	fflush( ofp );
	fflush( stderr );
	return((int)( errorIndicator * bytesProcessed ));
}
