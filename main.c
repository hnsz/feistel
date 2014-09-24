#include "main.h"

//      writes decryption or encryption of stdin to stdout
int main(int argc, char *argv[])
{
	FILE *inFile;
	FILE *outFile;

	char *inFilename = "stdin";
	char *outFilename = "stdout";
	char *password = "12345678";
	char keySchedule[10][4];
	char block[BLOCKSIZE];
	char *subKey;

	int decrypt;
	int i;



	/**	!!(REMOVE THIS COMMENT
	*	Using hardcoded pass for now
	*	Also changed function getArgs to skip password
	*/
	if(! getArgs(argc, argv, &decrypt, password)) {

		printUsageExit(argv[0], -1);

	}






/*
	For now
	Setting inFile = stdin
		outFile = stdout 
	inFile = fCheckOpen(inFilename, "r");
	outFile = fCheckOpen(outFilename, "w");
*/
	inFile = stdin;
	outFile = stdout;






	//	DEBUG
//	printf("password: %s\ndecrypt: %d\n", password, decrypt);




	buildKeySchedule(password, keySchedule);
	//	DEBUG
	//fPrintKeySchedule(keySchedule, 10, stdout);



	while ((fReadBlock(block, BLOCKSIZE, inFile))) {

		//	DEBUG
		//fprintf(outFile, "Next block:\n");
		//	END DEBUG
		for (i = 0; i < 10; i++) {
		//	DEBUG
		//fPrintBlock(block, outFile);
		//fprintf(outFile, "\n");
		//	END DEBUG
		 
			if (decrypt) {
				subKey = keySchedule[9 - i];
			} else {
				subKey = keySchedule[i];
			}

			feistel(block, subKey);
			printf("\nsubkey:");
			fPrintSubKey(stdout, subKey);


		}

		fPrintBlock(outFile,block);

	}

	//	PRINT TRAILING NEWLING
		printf("\n");



	fclose(inFile);
	fclose(outFile);


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

		if(i > 0)
			fputc(' ', out);

		fPrintNibbles(out, key[i]);
	}
	
}
//	Prints a byte as two nibbles in hex format
void fPrintNibbles(FILE *out, char ch)
{
	fprintf(out,"%0x%0x", toNibbleL(ch), toNibbleR(ch));
}

void buildKeySchedule(char *password, char keySchedule[][4])
{
	unsigned char cur_key[SHA_DIGEST_LENGTH];
	unsigned char next_key[SHA_DIGEST_LENGTH];
	int i;




	SHA1((unsigned const char *) password, strlen(password), next_key);



	for (i = 0; i < 5; i++) {


		memcpy(cur_key, next_key, SHA_DIGEST_LENGTH);

		memcpy(keySchedule[i * 2], cur_key, 4);
		memcpy(keySchedule[(i * 2) + 1], &(cur_key[4]), 4);


		SHA1(cur_key, SHA_DIGEST_LENGTH, next_key);
	}
}






void fPrintBlock(FILE *out, char block[BLOCKSIZE])
{
	int i;
	for (i = 0; i < BLOCKSIZE; i++) {
		fputc(block[i], out);
	}
}


void nPadN(char *block, int size, int n)
{
	int i;

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


	if(n < size && n > 0) {
		nPadN(block, size, n);
	}

	return n;
}

int getArgs(int argc, char **argv, int *decrypt, char *password)
{
	int pwLength;


	if (strcmp("-d", argv[1]) == 0) {

		*decrypt = 1;
	}
	else
	if(strcmp("-e", argv[1]) == 0) {

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
