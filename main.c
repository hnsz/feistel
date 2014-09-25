#include "main.h"
#define USE_CBC 0

//      writes decryption or encryption to "outfile"
int main(int argc, char *argv[])
{
	FILE *inFile;
	FILE *outFile;

	char *outFilename = "outfile";
	char keySchedule[10][4];

	int decrypt;
	char *IV = "92837465";
	char *password = "\x1e\xfe\xba\x22\xe7\x70\xe9\x5f";

	//char *ciphertextFile = "message.enc";
	char *ciphertextFile = "feistel.enc";
	char *plaintextFile = "message.txt";

	/**	!!(REMOVE THIS COMMENT
	*	Using hardcoded pass for now
	*	Also changed function getArgs to skip password
	*/
	if(! getArgs(argc, argv, &decrypt, password)) {

		printUsageExit(argv[0], -1);

	}





	if(decrypt) {
		inFile = fCheckOpen(ciphertextFile, "r");
	}
	else {
		inFile = fCheckOpen(plaintextFile, "r");
	}
	outFile = fCheckOpen(outFilename, "w");






	//	DEBUG
	printf("password: %s\nmode: %s\n", password, (decrypt)?"decrypting":"encrypting");



	
	buildKeySchedule(password, keySchedule);
	//	DEBUG
	fPrintKeySchedule(stdout, keySchedule, 10);


	if(USE_CBC)
		cbcXcrypt(inFile, outFile, decrypt, keySchedule, IV);
	else
		ecbXcrypt(inFile, outFile,  decrypt, keySchedule);




	fclose(inFile);
	fclose(outFile);


	return 0;
}
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
void cbcXcrypt(FILE *inFile, FILE *outFile, int decrypt, char keySchedule[][BLOCKSIZE/2], char IV[BLOCKSIZE])
{
	char block[BLOCKSIZE];
	char cbcMask[BLOCKSIZE];
	char nextCbcMask[BLOCKSIZE];
	int n;

	memcpy(cbcMask, IV, BLOCKSIZE);
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

char *allocCharBuffer(int size)
{
	char *buffer;

	buffer = malloc(size * sizeof(char));
	if (buffer == NULL) {
		printf("Cannot allocate memory.");
		exit(-1);
	}

	return buffer;
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
//	Prints a byte as two nibbles in hex format
void fPrintNibbles(FILE *out, char ch)
{
	fprintf(out,"%0x%0x", toNibbleL(ch), toNibbleR(ch));
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
			sprintf((key_hex + j * 2), "%02x", key_sha[j]);
		}

		memcpy(keySchedule[i * 2], key_hex, 4);
		memcpy(keySchedule[(i * 2) + 1], &(key_hex[4]), 4);


		SHA1((unsigned const char *)key_hex, 2 * SHA_DIGEST_LENGTH, key_sha);
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
	printf("n padding with: %d\n", n);

	for(i = size - n; i < size; i++) {

		block[i] = n;
	}
}

unsigned char toNibbleL(char num)
{
	unsigned char l;

	l = (unsigned char)num;
	return (l >> 4);
	
}
unsigned char toNibbleR(char num)
{
	unsigned char r;
	unsigned char mask  = 128+64+32+16;

	r = (unsigned char)num;

	return ((r | mask) ^ mask);
}

/*******
	!!(Remove Comment) There could be one off problems.
*/
int fReadBlock(char *block, int size, FILE *fp)
{
	int n;

	n = fread(block, 1, size, fp);
	

	//	DEBUG
	printf("Last fread returned: %d\n", n);
	//	END DEBUG
	if(n < size && n > 0) {
		nPadN(block, size, BLOCKSIZE - n);
	}

	return n;
}

int getArgs(int argc, char **argv, int *decrypt, char *password)
{
	int pwLength;


	if (strncmp("-d", argv[1], 2) == 0) {

		*decrypt = 1;
	}
	else
	if(strncmp("-e", argv[1], 2) == 0) {

		*decrypt = 0;
	}
	else {
		return 0;
	}

	//	!!(REMOVE COMMENT) must remove this return statement
	return 1;

	pwLength = strlen(argv[2]);
	if(pwLength >= 8) {
		
		password = allocCharBuffer(pwLength + 1);
		strcpy(password, argv[2]);
	}
	else {
		return 0;
	}

	return 1;
}


void printUsageExit(char *programName, int exitCode)
{
	printf("Usage: %s -e|-d password\n\
-d	decrypt\n-e	encrypt\nPassword must be of length > 8.\n", programName);
	exit(exitCode);
}




FILE *fCheckOpen(char *filename, char *mode)
{
	FILE *fp;

	fp = fopen(filename, mode);
	if(fp == NULL) {
		printf("Failed to open file '%s', mode \"%s\". Exit..", filename, mode);
		exit(-1);
	}

	return fp;
}
