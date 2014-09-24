#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>

#include "feistel.h"

#define BLOCKSIZE 8
#define KEYSIZE (BLOCKSIZE/2)


char *allocCharBuffer(int);
void buildKeySchedule(char *, char [][4]);
int getArgs(int, char **, int *, char *);
void printUsageExit(char *, int);
void fPrintBlock(FILE *, char [BLOCKSIZE], int);
FILE *fCheckOpen(char *, char *);
void nPadN(char *, int, int);
unsigned char toNibbleL(char);
unsigned char toNibbleR(char);
void fPrintKeySchedule(FILE *,char [][4], int);
void fPrintSubKey(FILE *, char [4]);
void fPrintNibbles(FILE *, char);
int fReadBlock(char *, int, FILE *);
int getPaddingSize(char *);
