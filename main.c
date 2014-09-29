#include "blockcipher.h"



int main(int argc, char *argv[])
{
	FILE *inFile;
	FILE *outFile;

	char *iv = "92837465";
	char *password = "Feistel";
	int direction;

	if(argc < 2)
		return -1;


	if(strcmp(argv[1], "-d") == 0)
		direction = 1;
	else
	if(strcmp(argv[1], "-e") == 0)
		direction = 0;
	else
		return -1;



	inFile = stdin;
	outFile = stdout;
	if(direction)
		decrypt(stdin, stdout, password, iv);
	else
		encrypt(stdin, stdout, password, iv);



	if(inFile != stdin)
		fclose(inFile);
	if(outFile != stdout)
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


FILE *fCheckOpen(char *filename, char *mode)
{
	FILE *fp;

	fp = fopen(filename, mode);
	if(fp == NULL) {
		printf("Failed to open file '%s', mode \"%s\". Exit..", 
		filename, mode);
		exit(-1);
	}

	return fp;
}
