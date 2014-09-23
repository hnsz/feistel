#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>

#include "feistel.h"

#define BLOCKSIZE 8


char *alloc_char_buffer(int size)
{
	char *buffer;

	buffer = malloc(size*sizeof(char));
	if(buffer == NULL) {
		printf("Cannot allocate memory.");
		exit(-1);
	}
	
	return buffer;
}


//	returns 0 and reports incorrect usage
int incorrectUsage(char* programName) 
{
    printf("usage: %s [-d] password\nWhere the password is 8 characters or more.\n", programName); // 8 should be 8 or longer!!!
    return 0;
}


//	returns lenght of string
size_t length(char* password)
{
      int i = 0;
      while (password[i] != '\0')
      {
	  i++;
      }
      return i;
}


// 	makes new key from old one
void computeNewKey(unsigned char key[20])
{
  
  
    int k;
    char pieceOfKeyString[3] = "";
    char keyString[(SHA_DIGEST_LENGTH*2) +1] = "";
    
    //char* probeersel = "\xb1\x40\xbe\x15\x4f\xd8\xaa\xd2\xc9\x4b\x0d\x48\x0a\x31\x63\x81\xf4\x96\xec\x9c";
    
    for (k = 0; k < SHA_DIGEST_LENGTH; k++) 
    {
	sprintf(pieceOfKeyString, "%02x", /*probeersel[k]*/key[k]);
	strcat(keyString, pieceOfKeyString);
    }
    
    SHA1((unsigned const char*) keyString , SHA_DIGEST_LENGTH*2, key);
    
    printf("%s = keystring\n", keyString);
    
    for (int i=0; i<SHA_DIGEST_LENGTH; i++)  	 	//for debugging purposes
    {							//
    printf("%02x ", key[i]);				//
    }							//
    printf("\n"); 					//for debugging purposes
    
}


void buildKeySchedule(char* password, char keySchedule[10][4])
{
    int i;
    unsigned char key[SHA_DIGEST_LENGTH];
    
    SHA1((unsigned const char*) password, length(password), key);
    
    for (int i=0; i<SHA_DIGEST_LENGTH; i++)  	 	//for debugging purposes
    {							//
    printf("%02x ", key[i]);				//
    }							//
    printf("\n"); 					//for debugging purposes

    for(i = 0; i<4; i++) 
    {  
	 memcpy(keySchedule[i*2], key, 4);
	 memcpy(keySchedule[(i*2)+1], key+4, 4);
	 
	 //int j;					// debugging
	 //for (int j=0; j<4; j++)  	 	//for debugging purposes
	 //{							//
	 //printf("%02x ", keySchedule[i*2][j]);				//
	 //}							//
	 printf("\n"); 						//for debugging purposes
	 
	 computeNewKey(key);
    }
}



//	checks usage
int correctUsage(int argc, char *argv[], int *decrypt, int *locationPassword)
{
    int i;
    for(i=1; i<argc; i++) 
    {
	if (strcmp("-d", argv[i]) == 0) {
	    if (*decrypt == 0) 
	    {
		*decrypt = 1;
	    } 
	    else 
	    {
		return incorrectUsage(argv[0]);
	    }
	} 
	else 
	{
	    if (strlen(argv[i]) >= 8) 
	    {
		if (*locationPassword == 0)
		{
		    *locationPassword = i;
		}
		else
		{
		    return incorrectUsage(argv[0]);
		}
	    } 
	    else 
	    {
		return incorrectUsage(argv[0]);
	    }
	}
    }
    
    if (*locationPassword == 0)
    {
	return incorrectUsage(argv[0]);
    }
    
    return 1;
}



//	prints block to file
void printToFile(char block[BLOCKSIZE], FILE *output)
{
    int i;
    for (i=0; i<BLOCKSIZE; i++)
    {
	fputc(block[i], output);
    }
}

//	writes decryption or encryption of stdin to stdout
int main(int argc, char *argv[])
{
    FILE *input, *output;
    int decrypt = 0, locationPassword = 0;
    
    
    
    if ( correctUsage(argc, argv, &decrypt, &locationPassword) )
    {	
	
	input = fopen("stdin", "r");
	
	if (input == NULL) 
	{
	    printf( "Could not open file: stdin\n" );
	}
    
	output = fopen("stdout", "w");
	if (output == NULL) 
	{
	    printf( "Could not open file: stdout\n" );
	}
	
	
	if (input != NULL && output != NULL)
	{
	    char* password = "\x1e\xfe\xba\x22\xe7\x70\xe9\x5f";  //argv[locationPassword];
	    printf("password: %s\ndecrypt: %d\n", password, decrypt);	//for debugging purposes
	    
	    char keySchedule[10][4];
	    buildKeySchedule(password, keySchedule);

	    char x;
	    int i = 0;
	    char block[BLOCKSIZE];
	    
	    
	    while ((x = fgetc(input)) != EOF) 
	    {
		
		block[i] = x;
		
		if (i == 7) 
		{
		    int j;
		    for (j=0; j<10; j++)
		    {
			char* subKey;
			if (decrypt) {
			  subKey = keySchedule[9-j];
			} else {
			  subKey = keySchedule[j];
			}
			
			feistel(block, subKey);
			
		    }
		    printToFile(block, output);
		    //fprintf(output, "%s", block);
		    //fprintf(output, "\n");				//for debugging purposes
		    
		    i = 0;
		    
		} 
		else 
		{
		    i++;
		}
	    }

	    
	    
	    
	    fclose(input);
	    fclose(output);
	}
	
    } 
    
    return 0;
}



