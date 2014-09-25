#include "blockcipher.h"

#define ENCRYPT 0
#define DECRYPT 1


/*******
*       You should use these.
*		encrypt()
*		decrypt()		
*       Both functions take the same arguments.
*               FILE *  Pointer to inputfile
*               FILE *  Pointer to outputfile
*               char *  A null terminated keyphrase / password
*               char *  Pointer to an 8 byte initialisation vector or NULL 
*                       If NULL is given, en-/decryption is done in 
*                       ecb mode (electronic codebook)
*                       If an 8 byte initialisation vector is given
*                       en-/decryption is done in cbc mode 
*                       (cypherblock chaining)
*
*	Debug info is printed to stderr.
*
*/

void encrypt(FILE *inFile, FILE *outFile, char *passkey, char *iv)
{
	char keySchedule[10][4];

	buildKeySchedule(passkey, keySchedule);

	//	DEBUG
	fprintf(stderr, "password: %s\nmode: %s\n", passkey, "encrypting");
	fPrintKeySchedule(stderr, keySchedule, 10);
	//	END DEBUG



	if(iv == NULL)
		ecbXcrypt(inFile, outFile, ENCRYPT, keySchedule);
	else
		cbcXcrypt(inFile, outFile, ENCRYPT, keySchedule, iv);


	fprintf(stderr,"\n\n");
}


void decrypt(FILE *inFile, FILE *outFile, char *passkey, char *iv)
{
	char keySchedule[10][4];

	buildKeySchedule(passkey, keySchedule);

	//	DEBUG
	fprintf(stderr, "password: %s\nmode: %s\n", passkey, "decrypting");
	fPrintKeySchedule(stderr, keySchedule, 10);
	//	END DEBUG




	if(iv == NULL)
		ecbXcrypt(inFile, outFile, DECRYPT, keySchedule);
	else
		cbcXcrypt(inFile, outFile, DECRYPT, keySchedule, iv);


	fprintf(stderr,"\n\n");
}









/*******
*	You shouldn have to use these.
*/
void ecbXcrypt(FILE *inFile, FILE *outFile, int decrypt, char keySchedule[][BLOCKSIZE/2])
{
	char block[BLOCKSIZE];
	int n;
	while ((fReadBlock(block, BLOCKSIZE, inFile))) {

		if (decrypt)
			decryptBlock(block, keySchedule);
		else
			encryptBlock(block, keySchedule);
		
		
		if(decrypt && (n = getPaddingSize(block)))
			fPrintBlock(outFile, block, BLOCKSIZE - n);
		else
			fPrintBlock(outFile, block, BLOCKSIZE);
	}
}
void cbcXcrypt(FILE *inFile, FILE *outFile, int decrypt, char keySchedule[][BLOCKSIZE/2], char *iv)
{
	char block[BLOCKSIZE];
	char cbcMask[BLOCKSIZE];
	char nextCbcMask[BLOCKSIZE];
	int n;

	memcpy(cbcMask, iv, BLOCKSIZE);
	while ((fReadBlock(block, BLOCKSIZE, inFile))) {

		if (decrypt) {
			memcpy(nextCbcMask, block, BLOCKSIZE);
			decryptBlock(block, keySchedule);
			xorBlock(block, cbcMask);
		} else {
			xorBlock(block, cbcMask);
			encryptBlock(block, keySchedule);
			memcpy(nextCbcMask, block, BLOCKSIZE);
		}

		memcpy(cbcMask, nextCbcMask, BLOCKSIZE);

		if(decrypt && (n = getPaddingSize(block)))
			fPrintBlock(outFile, block, BLOCKSIZE - n);
		else
			fPrintBlock(outFile, block, BLOCKSIZE);

	}
}


void encryptBlock(char block[BLOCKSIZE], char keySchedule[][BLOCKSIZE/2])
{
	int i;
	for (i = 0; i < 10; i++) {
	 
		feistelDecrypt(block, keySchedule[i]);

	}
}
void decryptBlock(char block[BLOCKSIZE], char keySchedule[][BLOCKSIZE/2])
{
	int i;
	for (i = 0; i < 10; i++) {
	 
		feistelEncrypt(block, keySchedule[9-i]);
	}
}
void xorBlock(char block[BLOCKSIZE], char mask[BLOCKSIZE])
{
	int i;

	for(i = 0; i < BLOCKSIZE; ++i) {

		block[i] ^= mask[i];
	}
}
int getPaddingSize(char *block)
{
	if(block[BLOCKSIZE - 1] > 0 && block[BLOCKSIZE - 1] < BLOCKSIZE)
		return block[BLOCKSIZE - 1];
	else
		return 0;
}


void fPrintKeySchedule(FILE *out, char keySchedule[][KEYSIZE], int nKeys)
{
	int i;

	for(i = 0; i < nKeys; ++i) {

		fPrintSubKey(out, keySchedule[i]);
		fprintf(out, "\n");
	}
}

void fPrintSubKey(FILE *out, char key[KEYSIZE])
{
	int i;

	for(i = 0; i < KEYSIZE; ++i) {

		fputc(key[i], out);
	}
	
}


void buildKeySchedule(char *password, char keySchedule[][4])
{
	unsigned char key_sha[SHA_DIGEST_LENGTH];
	int i;
	int j;
	char key_hex[2 * SHA_DIGEST_LENGTH + 1];


	SHA1((unsigned const char *) password, strlen(password), key_sha);

	

	for (i = 0; i < 5; i++) {
		for(j = 0; j < SHA_DIGEST_LENGTH; ++j) {
			sprintf(key_hex +j*2 , "%02x", key_sha[j]);
		}
		fprintf(stderr, "%s\n", key_hex);

		memcpy(keySchedule[i * 2], key_hex, 4);
		memcpy(keySchedule[(i * 2) + 1], &(key_hex[4]), 4);
		SHA1((unsigned const char *)key_hex, 2 * SHA_DIGEST_LENGTH , key_sha);
	}
}


void fPrintBlock(FILE *out, char block[BLOCKSIZE], int n)
{
	int i;
	for (i = 0; i < n; i++) {
		fputc(block[i], out);
	}
}


void nPadN(char *block, int size, int n)
{
	int i;
	fprintf(stderr, "n padding with: %d\n", n);

	for(i = size - n; i < size; i++) {

		block[i] = n;
	}
}


int fReadBlock(char *block, int size, FILE *fp)
{
	int n;

	n = fread(block, 1, size, fp);
	

	if(n < size && n > 0) {
		nPadN(block, size, BLOCKSIZE - n);
	}

	return n;
}

