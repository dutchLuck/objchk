/*
 * C O N F I G . C
 *
 * Last Modified on Mon Jun 23 19:14:05 2025
 *
 * 2025-Jun-22 Added Chr option handling
 */

#include <stdio.h>    /* printf() */
#include <stdlib.h>   /* floating point double */
#include <unistd.h>   /* getopt() */
#include <ctype.h>    /* isprint()*/
#include <limits.h>   /* INT_MIN INT_MAX LONG_MIN LONG_MAX */
#include <float.h>    /* DBL_MIN DBL_MAX */
#include <string.h>   /* strchr() */
#include "config.h"   /* struct config */
#include "genFun.h"   /* convertOptionStringToInteger() limitIntegerValueToEqualOrWithinRange() */


int  configureIntegerOption( struct optInt *  intStructPtr, char *  intString ) {
  intStructPtr->active = TRUE;
  intStructPtr->optionInt = convertOptionStringToInteger( intStructPtr->defaultVal, intString,
    intStructPtr->optID, &intStructPtr->active, TRUE );
  intStructPtr->optionInt = limitIntegerValueToEqualOrWithinRange( intStructPtr->optionInt,
    intStructPtr->mostNegLimit, intStructPtr->mostPosLimit );
  return( intStructPtr->active );
}


int  configureLongOption( struct optLng *  longStructPtr, char *  longString ) {
  longStructPtr->active = TRUE;
  longStructPtr->optionLng = convertOptionStringToLong( longStructPtr->defaultVal, longString,
    longStructPtr->optID, &longStructPtr->active, TRUE );
  longStructPtr->optionLng = limitLongValueToEqualOrWithinRange( longStructPtr->optionLng,
    longStructPtr->mostNegLimit, longStructPtr->mostPosLimit );
  return( longStructPtr->active );
}


int  configureDoubleOption( struct optDbl *  dblStructPtr, char *  dblString ) {
  dblStructPtr->active = TRUE;
  dblStructPtr->optionDbl = convertOptionStringToDouble( dblStructPtr->defaultVal, dblString,
    dblStructPtr->optID, &dblStructPtr->active, TRUE );
  dblStructPtr->optionDbl = limitDoubleValueToEqualOrWithinRange( dblStructPtr->optionDbl,
    dblStructPtr->mostNegLimit, dblStructPtr->mostPosLimit );
  return( dblStructPtr->active );
}


int  configureChrOption( struct optChr *  chrStructPtr, char *  chrString )  {
  size_t  len;

  len = strlen( chrString );
  chrStructPtr->active = TRUE;
  chrStructPtr->optionChr = ( int ) *chrString;
  if (( *chrString == '\\' ) && ( len > ( size_t ) 1 ) && ( chrString[ 1 ] != '\\'))  {
    switch ( chrString[ 1 ] )  {
      case '0' : chrStructPtr->optionChr = '\0'; break;   /* NULL */
      case 'a' : chrStructPtr->optionChr = '\a'; break;   /* audible bell */
      case 'b' : chrStructPtr->optionChr = '\b'; break;   /* backspace */
      case 'f' : chrStructPtr->optionChr = '\f'; break;   /* form-feed */
      case 'n' : chrStructPtr->optionChr = '\n'; break;   /* newline */
      case 'r' : chrStructPtr->optionChr = '\r'; break;   /* carriage return */
      case 't' : chrStructPtr->optionChr = '\t'; break;   /* horizontal tab */
      case 'x' :
      case 'X' : {
        if ( len > ( size_t ) 3 )  {
          chrStructPtr->optionChr = convert2HexChrNibblesToInt( chrString + 2 );
        }
        else if ( len > ( size_t ) 2 )  {
          chrStructPtr->optionChr = convertHexChrNibbleToInt( chrString + 2 );
        }
        break;
      }
      default :  chrStructPtr->optionChr = ','; break;    /* reinstate comma */
    }
    chrStructPtr->optionChr = limitIntegerValueToEqualOrWithinRange( chrStructPtr->optionChr , 0, 127 );
  }
  return( chrStructPtr->active );
}

// Functions for Command Line Options Configuration from JSON Data
void  usage( struct config *  opt, char *  exeName )  {
  printf( "Usage:\n");
  printf( " %s [-a][-A][-D][-h][-i][-m][-o TXT][-s][-v][-V][-w INT]\n", exeName );
  printf( " %s %s\n", opt->a.optID, opt->a.helpStr ); /* address */
  printf( " %s %s\n", opt->A.optID, opt->A.helpStr ); /* ascii */
  printf( " %s %s\n", opt->D.optID, opt->D.helpStr ); /* debug */
  printf( " %s %s\n", opt->h.optID, opt->h.helpStr ); /* help */
  printf( " %s %s\n", opt->i.optID, opt->i.helpStr ); /* intel */
  printf( " %s %s\n", opt->m.optID, opt->m.helpStr ); /* motorola */
  printf( " %s %s\n", opt->o.optID, opt->o.helpStr ); /* output */
  printf( " %s %s\n", opt->s.optID, opt->s.helpStr ); /* space */
  printf( " %s %s\n", opt->v.optID, opt->v.helpStr ); /* verbose */
  printf( " %s %s\n", opt->V.optID, opt->V.helpStr ); /* version */
  printf( " %s %s\n", opt->w.optID, opt->w.helpStr ); /* width */
}

void  initConfiguration ( struct config *  opt )  {
// address: optFlg
  opt->a.active = FALSE;
  opt->a.optID = "-a";
  opt->a.helpStr = "...... disable address output mode";
// ascii: optFlg
  opt->A.active = FALSE;
  opt->A.optID = "-A";
  opt->A.helpStr = "...... disable ascii output mode";
// debug: optFlg
  opt->D.active = FALSE;
  opt->D.optID = "-D";
  opt->D.helpStr = "...... enable debug output mode";
// help: optFlg
  opt->h.active = FALSE;
  opt->h.optID = "-h";
  opt->h.helpStr = "...... this help / usage information";
// intel: optFlg
  opt->i.active = FALSE;
  opt->i.optID = "-i";
  opt->i.helpStr = "...... expect intel object format - not implemented yet";
// motorola: optFlg
  opt->m.active = FALSE;
  opt->m.optID = "-m";
  opt->m.helpStr = "...... expect motorola object format - not implemented yet";
// output: optStr
  opt->o.active = FALSE;
  opt->o.optID = "-o";
  opt->o.helpStr = "TXT .. send the output to a file named 'TXT'";
  opt->o.optionStr = "";
// space: optFlg
  opt->s.active = FALSE;
  opt->s.optID = "-s";
  opt->s.helpStr = "...... enable space as a printable character in the output";
// verbose: optFlg
  opt->v.active = FALSE;
  opt->v.optID = "-v";
  opt->v.helpStr = "...... enable more verbose information output";
// version: optFlg
  opt->V.active = FALSE;
  opt->V.optID = "-V";
  opt->V.helpStr = "...... enable version information output";
// width: optInt
  opt->w.active = FALSE;
  opt->w.optID = "-w";
  opt->w.helpStr = "INT .. output INT bytes per line - where 1 <= INT <= 32";
  opt->w.mostPosLimit = 32;
  opt->w.mostNegLimit = 1;
  opt->w.optionInt = 16;
  opt->w.defaultVal = 16;
}

int  setConfiguration ( int  argc, char *  argv[], struct config *  opt )  {
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, OPTIONS )) != -1 ) {
    switch ( c ) {
      case 'a': opt->a.active = TRUE; break; /* no_address */
      case 'A': opt->A.active = TRUE; break; /* no_ascii */
      case 'D': opt->D.active = TRUE; break; /* debug */
      case 'h': opt->h.active = TRUE; break; /* help */
      case 'i': opt->i.active = TRUE; break; /* intel */
      case 'm': opt->m.active = TRUE; break; /* motorola */
      case 'o': opt->o.active = TRUE; opt->o.optionStr = optarg; break; /* output_file */
      case 's': opt->s.active = TRUE; break; /* space */
      case 'v': opt->v.active = TRUE; break; /* verbose */
      case 'V': opt->V.active = TRUE; break; /* version */
      case 'w': configureIntegerOption( &opt->w, optarg ); break; /* width */
      case '?' : {
        if ( strchr( "ow", optopt ) != NULL ) {
          fprintf (stderr, "Error: Option -%c requires an argument.\n", optopt);
          switch ( optopt ) {
            case 'o': opt->o.active = FALSE; break;
            case 'w': opt->w.active = FALSE; break;
          }
        }
        else if (isprint (optopt))
          fprintf (stderr, "Warning: Unknown option \"-%c\" ? - ignoring it!\n", optopt);
        else
          fprintf (stderr, "Warning: Unknown non-printable option character 0x%x ? - ignoring it!\n", optopt);
        break;
      }
    }
  }
  return( optind );
}

void  configuration_status( struct config *  opt )  {
  printf( "Debug: option -a is %sctive (-a %s)\n", (opt->a.active) ? "a" : "ina", opt->a.helpStr); /* address */
  printf( "Debug: option -A is %sctive (-A %s)\n", (opt->A.active) ? "a" : "ina", opt->A.helpStr); /* ascii */
  printf( "Debug: option -D is %sctive (-D %s)\n", (opt->D.active) ? "a" : "ina", opt->D.helpStr); /* debug */
  printf( "Debug: option -h is %sctive (-h %s)\n", (opt->h.active) ? "a" : "ina", opt->h.helpStr); /* help */
  printf( "Debug: option -i is %sctive (-i %s)\n", (opt->i.active) ? "a" : "ina", opt->i.helpStr); /* intel */
  printf( "Debug: option -m is %sctive (-m %s)\n", (opt->m.active) ? "a" : "ina", opt->m.helpStr); /* motorola */
  printf( "Debug: option -o is %sctive (-o %s)\n", (opt->o.active) ? "a" : "ina", opt->o.helpStr); /* output */
  printf( "Debug: option -o value is \"%s\"\n", opt->o.optionStr); /* output */
  printf( "Debug: option -s is %sctive (-s %s)\n", (opt->s.active) ? "a" : "ina", opt->s.helpStr); /* space */
  printf( "Debug: option -v is %sctive (-v %s)\n", (opt->v.active) ? "a" : "ina", opt->v.helpStr); /* verbose */
  printf( "Debug: option -V is %sctive (-V %s)\n", (opt->V.active) ? "a" : "ina", opt->V.helpStr); /* version */
  printf( "Debug: option -w is %sctive (-w %s)\n", (opt->w.active) ? "a" : "ina", opt->w.helpStr); /* width */
  printf( "Debug: option -w value is %d, limits: %d .. %d\n", opt->w.optionInt, opt->w.mostNegLimit, opt->w.mostPosLimit); /* width */
}
