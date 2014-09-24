#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>

#include "feistel.h"

void feistelEncrypt(char *block, char *subKey)
{
	char LH[4];
	char RH[4];
	char LHnew[4];
	char RHnew[4];
	int i;

	//Split block in halves
	memcpy(LH, block, 4);
	memcpy(RH, block + 4, 4);


	//Encrypt
	memcpy(LHnew, RH, 4);
	for (i = 0; i < 4; i++) {
		RHnew[i] = LH[i] ^ subKey[i];
	}

	//Put block back together
	memcpy(block, LHnew, 4);
	memcpy(block + 4, RHnew, 4);

}
void feistelDecrypt(char *block, char *subKey)
{
	char LH[4];
	char RH[4];
	char LHnew[4];
	char RHnew[4];
	int i;

	//Split block in halves
	memcpy(LH, block, 4);
	memcpy(RH, block + 4, 4);


	//Encrypt
	memcpy(RHnew, LH, 4);
	for (i = 0; i < 4; i++) {
		LHnew[i] = RH[i] ^ subKey[i];
	}

	//Put block back together
	memcpy(block, LHnew, 4);
	memcpy(block + 4, RHnew, 4);

}
