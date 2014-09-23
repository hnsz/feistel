#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>

#include "feistel.h"

#define BLOCKSIZE 8


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


//      returns lenght of string
size_t length(char *password)
{
	int i = 0;
	while (password[i] != '\0') {
		i++;
	}
	return i;
}


//      makes new key from old one
void computeNewKey(unsigned char key[20])
{


	int k;
	char pieceOfKeyString[3] = "";
	char keyString[(SHA_DIGEST_LENGTH * 2) + 1] = "";

	//char* probeersel = "\xb1\x40\xbe\x15\x4f\xd8\xaa\xd2\xc9\x4b\x0d\x48\x0a\x31\x63\x81\xf4\x96\xec\x9c";

	for (k = 0; k < SHA_DIGEST_LENGTH; k++) {
		sprintf(pieceOfKeyString, "%02x", /*probeersel[k] */
			key[k]);
		strcat(keyString, pieceOfKeyString);
	}

	SHA1((unsigned const char *) keyString, SHA_DIGEST_LENGTH * 2, key);

	printf("%s = keystring\n", keyString);

	//	DEBUG
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		printf("%02x ", key[i]);
	}
	printf("\n");
	//	END DEBUG

}


void buildKeySchedule(char *password, char keySchedule[10][4])
{
	int i;
	unsigned char key[SHA_DIGEST_LENGTH];

	SHA1((unsigned const char *) password, length(password), key);

	//	DEBUG
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		printf("%02x ", key[i]);
	}
	printf("\n");
	//	END DEBUG


	for (i = 0; i < 4; i++) {
		memcpy(keySchedule[i * 2], key, 4);
		memcpy(keySchedule[(i * 2) + 1], key + 4, 4);


		//	DEBUG
		/*
		int j;
		for (int j=0; j<4; j++) {
			printf("%02x ", keySchedule[i*2][j]);
		}
		printf("\n");
		*/
		//	END DEBUG

		computeNewKey(key);
	}
}



int getArgs(int argc, char **argv, int *decrypt, char *password)
{
	int pwLength;


	if (strcmp("-d", argv[0]) == 0) {

		*decrypt = 1;
	}
	else
	if(strcmp("-e", argv[0]) == 0) {

		*decrypt = 0;
	}
	else {
		return 0;
	}


	pwLength = strlen(argv[1]);
	if(pwLength >= 8) {
		
		password = allocCharBuffer(pwLength + 1);
		strcpy(password, argv[1]);
	}
	else {
		return 0;
	}

	return 1;
}


//      returns 0 and reports incorrect usage
void printUsageExit(char *programName, int exitCode)
{
	printf("Usage: %s -e|-d password\n\
-d	decrypt\n-e	encrypt\nPassword must be of length > 8.\n", programName);
	exit(exitCode);
}



void fPrintBlock(char block[BLOCKSIZE], FILE * output)
{
	int i;
	for (i = 0; i < BLOCKSIZE; i++) {
		fputc(block[i], output);
	}
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

void nPadN(char *block, int size, int n)
{
	int i;

	for(i = size - n; i < size; i++) {

		block[i] = n;
	}
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


	decrypt = 1;
	/*	Disable: Using Hardcoded values for now.
	if(! getArgs(argc, argv, &decrypt, password)) {

		printUsageExit(argv[0], -1);

	}
	*/
	inFile = fCheckOpen(inFilename, "r");
	outFile = fCheckOpen(outFilename, "w");

	//	DEBUG
	printf("password: %s\ndecrypt: %d\n", password, decrypt);




	buildKeySchedule(password, keySchedule);



	while ((fReadBlock(block, BLOCKSIZE, inFile)) != EOF) {

		for (i = 0; i < 10; i++) {
			if (decrypt) {
				subKey = keySchedule[9 - i];
			} else {
				subKey = keySchedule[i];
			}

			feistel(block, subKey);

		}
		fPrintBlock(block, outFile);

		//	DEBUG
		//fprintf(output, "%s", block);
		//fprintf(output, "\n");

	}




	fclose(inFile);
	fclose(outFile);


	return 0;
}