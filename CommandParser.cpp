#include "StdAfx.h"
#include "CommandParser.h"
#include "Keccak.h"

unsigned int hashType = 0;
unsigned int hashWidth = 512;
unsigned int sha3widths[] = {256, 512, 0};
unsigned int keccakwidths[] = {256, 512, 0};


int doFile(const char *fileName)
{
	if(hashType==0)
	{
		//  SHA-3
		unsigned int hashSize = hashWidth;
		keccakState *st = keccakCreate(hashSize);

		FILE *fHand = fopen(fileName, "rb");
		if(!fHand)
		{
			printf("Unable to open input file: %s\n", fileName);
			return 0;
		}
		fseek(fHand, 0, SEEK_END);
		unsigned int fileSize = ftell(fHand);
		fseek(fHand, 0, SEEK_SET);
		char *msg = new char[fileSize];
		fread(msg, 1, fileSize, fHand);
		fclose(fHand);
		printf("Loaded\n");
		keccakUpdate((uint8_t*)msg, 0, fileSize, st);
		delete[] msg;
		unsigned char *op = sha3Digest(st);

		printf("Output: ");
		for(unsigned int i=0 ; i!=(hashSize/8) ; i++)
		{
			printf("%.2x", *(op++));
		}
		printf("\n");
	}
	else if (hashType==1)
	{
		// Keccak
		unsigned int hashSize = hashWidth;
		keccakState *st = keccakCreate(hashSize);

		FILE *fHand = fopen(fileName, "rb");
		if(!fHand)
		{
			printf("Unable to open input file: %s\n", fileName);
			return 0;
		}
		fseek(fHand, 0, SEEK_END);
		unsigned int fileSize = ftell(fHand);
		fseek(fHand, 0, SEEK_SET);
		char *msg = new char[fileSize];
		fread(msg, 1, fileSize, fHand);
		fclose(fHand);
		printf("Loaded\n");
		keccakUpdate((uint8_t*)msg, 0, fileSize, st);
		delete[] msg;
		unsigned char *op = keccakDigest(st);

		printf("Output: ");
		for(unsigned int i=0 ; i!=(hashSize/8) ; i++)
		{
			printf("%.2x", *(op++));
		}
		printf("\n");	
	}
}

void parseParameter(const char *param)
{
	unsigned int index = 0;
	unsigned int paramSize = 0;

	paramSize = strlen(param);

	// Eat leading whitespace
	for(unsigned int i=index ; i!=paramSize ; i++)
	{
		const char posI = param[i];
		if((posI!=' ') && (posI!='\t'))
		{
			index = i;
			break;
		}
	}

	if(index!=paramSize)
	{
		if(param[index]!='-')
		{
			doFile(&param[index]);
		}
		else
		{
				
		}
	}
}

void parseCommandLine(const int argc, char* argv[])
{
	if(argc>1)
	{
		for(unsigned int i=1 ; i!=argc ; i++)
		{
			parseParameter(argv[i]);
		}	
	}
}