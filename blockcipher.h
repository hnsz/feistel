#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>

#include "feistel.h"

#define BLOCKSIZE 8
#define KEYSIZE (BLOCKSIZE/2)



/*******
*	You should use these.
*	Both functions take the same arguments.
*		FILE *	Pointer to inputfile
*		FILE *	Pointer to outputfile
*		char *	A null terminated keyphrase / password
*		char *	Pointer to an 8 byte initialisation vector or NULL 
*			If NULL is given, en-/decryption is done in 
*			ecb mode (electronic codebook)
*			If an 8 byte initialisation vector is given
*			en-/decryption is done in cbc mode 
*			(cypherblock chaining)
*
*	Debug info is printed to stderr
*
*/
void encrypt(FILE *, FILE *, char *, char *);
void decrypt(FILE *, FILE *, char *, char *);




/******
*	You shouldn't have to use these.
*/
void buildKeySchedule(char *, char [][BLOCKSIZE/2]);
void cbcXcrypt(FILE *, FILE *, int, char [][BLOCKSIZE/2], char *);
void ecbXcrypt(FILE *, FILE *, int, char [][BLOCKSIZE/2]);
void encryptBlock(char [BLOCKSIZE], char [][BLOCKSIZE/2]);
void decryptBlock(char [BLOCKSIZE], char [][BLOCKSIZE/2]);
int getPaddingSize(char *);
void fPrintBlock(FILE *, char [BLOCKSIZE], int);
void fPrintKeySchedule(FILE *,char [][BLOCKSIZE/2], int);
void fPrintSubKey(FILE *, char [BLOCKSIZE/2]);
int fReadBlock(char *, int, FILE *);
void nPadN(char *, int, int);
void xorBlock(char [BLOCKSIZE], char [BLOCKSIZE]);


