/*
 * C O N F I G . H
 *
 * Last Modified on Fri Jun 21 22:49:11 2024
 *
 */

#include <stdio.h>

#ifndef  OPTIONS_H
#define  OPTIONS_H

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!(FALSE))
#endif

struct optFlg {
  int active;
  char *  optID;
  char *  helpStr;
};

struct optChr {
  int active;
  char *  optID;
  char *  helpStr;
  int  optionChr;
};

struct optStr {
  int active;
  char *  optID;
  char *  helpStr;
  char *  optionStr;
};

struct optInt {
  int active;
  char *  optID;
  char *  helpStr;
  int defaultVal;
  int mostPosLimit;
  int mostNegLimit;
  int optionInt;
};

struct optLng {
  int active;
  char *  optID;
  char *  helpStr;
  long defaultVal;
  long mostPosLimit;
  long mostNegLimit;
  long optionLng;
};

struct optDbl {
  int active;
  char *  optID;
  char *  helpStr;
  double defaultVal;
  double mostPosLimit;
  double mostNegLimit;
  double optionDbl;
};

struct config {
  struct optFlg  a;
  struct optFlg  A;
  struct optFlg  D;
  struct optFlg  h;
  struct optStr  o;
  struct optFlg  s;
  struct optFlg  v;
  struct optInt  w;
};

#define OPTIONS "aADho:svw:"

void  usage( struct config *  optStructPtr, char *  exeName );
void  initConfiguration ( struct config *  optStructPtr );
int  setConfiguration ( int  argc, char *  argv[], struct config *  optStructPtr );

#endif
