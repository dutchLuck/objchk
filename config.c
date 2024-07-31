/*
 * C O N F I G . C
 *
 * Last Modified on Wed Jul 31 13:25:15 2024
 *
 */

#include <stdio.h>    /* printf() */
#include <stdlib.h>   /* floating point double */
#include <unistd.h>   /* getopt() */
#include <ctype.h>    /* isprint()*/
#include <limits.h>   /* INT_MIN INT_MAX LONG_MIN LONG_MAX */
#include <float.h>    /* DBL_MIN DBL_MAX */
#include <string.h>   /* strchr() */
#include "config.h"   /* struct config */
#include "genFun.h"   /* convertOptionStringToInteger() */


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


void  usage( struct config *  opt, char *  exeName )  {
  printf( "Usage:\n");
  printf( " %s [-a] [-A] [-D] [-h] [-o TXT] [-s] [-v] [-w INT][ fileName1 .. [ fileNameX ]]\n", exeName );
  printf( " %s %s\n", opt->a.optID, opt->a.helpStr );
  printf( " %s %s\n", opt->A.optID, opt->A.helpStr );
  printf( " %s %s\n", opt->D.optID, opt->D.helpStr );
  printf( " %s %s\n", opt->h.optID, opt->h.helpStr );
  printf( " %s %s\n", opt->i.optID, opt->i.helpStr );
  printf( " %s %s\n", opt->m.optID, opt->m.helpStr );
  printf( " %s %s\n", opt->o.optID, opt->o.helpStr );
  printf( " %s %s\n", opt->s.optID, opt->s.helpStr );
  printf( " %s %s\n", opt->v.optID, opt->v.helpStr );
  printf( " %s %s\n", opt->w.optID, opt->w.helpStr );
}


void  initConfiguration ( struct config *  opt )  {
  opt->a.active = FALSE;
  opt->a.optID = "-a";
  opt->a.helpStr = "...... disable address output mode";

  opt->A.active = FALSE;
  opt->A.optID = "-A";
  opt->A.helpStr = "...... disable ASCII output mode";

  opt->D.active = FALSE;
  opt->D.optID = "-D";
  opt->D.helpStr = "...... enable Debug output mode";

  opt->h.active = FALSE;
  opt->h.optID = "-h";
  opt->h.helpStr = "...... this help/usage information";

  opt->i.active = FALSE;
  opt->i.optID = "-i";
  opt->i.helpStr = "...... expect intel object format - not implemented yet";

  opt->m.active = FALSE;
  opt->m.optID = "-m";
  opt->m.helpStr = "...... expect motorola object format - not implemented yet";

  opt->o.active = FALSE;
  opt->o.optID = "-o";
  opt->o.helpStr = "TXT .. output the dump to an output file named \"TXT\" ";
  opt->o.optionStr = "";

  opt->s.active = FALSE;
  opt->s.optID = "-s";
  opt->s.helpStr = "...... enable space as a printable character in the output";

  opt->v.active = FALSE;
  opt->v.optID = "-v";
  opt->v.helpStr = "...... enable more verbose information output";

  opt->w.active = FALSE;
  opt->w.optID = "-w";
  opt->w.helpStr = "INT .. output INT bytes per line - where 1 <= INT <= 32";
  opt->w.mostPosLimit = 32;
  opt->w.mostNegLimit = 1;
  opt->w.defaultVal = 16;
  opt->w.optionInt = opt->w.defaultVal;
}


int  setConfiguration ( int  argc, char *  argv[], struct config *  opt )  {
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, OPTIONS )) != -1 ) {
    switch ( c ) {
      case 'a': opt->a.active = TRUE; break;
      case 'A': opt->A.active = TRUE; break;
      case 'D': opt->D.active = TRUE; break;
      case 'h': opt->h.active = TRUE; break;
      case 'i': opt->i.active = TRUE; break;
      case 'm': opt->m.active = TRUE; break;
      case 'o': opt->o.active = TRUE; opt->o.optionStr = optarg; break;
      case 's': opt->s.active = TRUE; break;
      case 'v': opt->v.active = TRUE; break;
      case 'w': configureIntegerOption( &opt->w, optarg ); break;
      case '?': {
        if ( strchr( "ow", optopt ) != NULL ) {
          fprintf (stderr, "Error: Option -%c requires an argument.\n", optopt);
          if ( optopt == 'o' )  opt->o.active = FALSE;
          if ( optopt == 'w' )  opt->w.active = FALSE;
        }
        else if (isprint (optopt))
          fprintf (stderr, "Warning: Unknown option \"-%c\".\n", optopt);
        else
          fprintf (stderr, "Warning: Unknown option character `\\x%x'.\n", optopt);
        break;
      }
    }
  }
  return( optind );
}
