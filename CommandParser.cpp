#include "StdAfx.h"
#include "CommandParser.h"
#include "Keccak.h"

unsigned int hashType = 0;
unsigned int hashWidth = 512;
unsigned int sha3widths[] = {224, 256, 384, 512, 0};
unsigned int keccakwidths[] = {224, 256, 384, 512, 0};


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

		printf("SHA-3-%u %s: ", hashSize, fileName);
		for(unsigned int i=0 ; i!=(hashSize/8) ; i++)
		{
			printf("%.2x", *(op++));
		}
		printf("\n");
		return 1;
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

		printf("Keccak-%u %s: ", hashSize, fileName);
		for(unsigned int i=0 ; i!=(hashSize/8) ; i++)
		{
			printf("%.2x", *(op++));
		}
		printf("\n");
		return 1;
	}
}

int parseAlg(const char *param, const unsigned int pSize)
{
	unsigned int index = 0;
	if(param[index] == '=')
	{
		index++;
	}

	if(index+1 == pSize)
	{
		const char algInitial = param[index];
		if(algInitial == 'k')
		{
			hashType = 1;
			return 1;
		}
		else if(algInitial == 's')
		{
			hashType = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int parseWidth(const char *param, const unsigned int pSize)
{
	unsigned int index = 0;
	if(param[index] == '=')
	{
		index++;
	}

	if(index+3 == pSize)
	{
		if(strncmp(&param[index], "224", pSize-index)==0)
		{
			hashWidth = 224;
			return 1;
		}
		else if(strncmp(&param[index], "256", pSize-index)==0)
		{
			hashWidth = 256;
			return 1;	
		}
		else if(strncmp(&param[index], "384", pSize-index)==0)
		{
			hashWidth = 384;
			return 1;
		}
		else if(strncmp(&param[index], "512", pSize-index)==0)
		{
			hashWidth = 512;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}

int parseOption(const char *param, const unsigned int pSize)
{
	unsigned int index = 1;

	if(index!=pSize)
	{
		const char commandInitial = param[index];
		if(commandInitial=='h')
		{
			if((index+1) == pSize)
			{
				// Usage
			}
			else
			{
				return 0;
			}
		}
		else if(commandInitial=='a')
		{
			return parseAlg(&param[index+1], pSize-(index+1));	
		}
		else if(commandInitial=='w')
		{
			return parseWidth(&param[index+1], pSize-(index+1));
		}
		else
		{
			return 0;	
		}
	}
	else 
	{
		return 0;
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
			parseOption(&param[index], paramSize-index);	
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