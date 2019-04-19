#include "stdafx.h"
#include "Keccak.h"
#include "CommandParser.h"

#include "Endian.h"
#include "Rotation.h"

// Constants of the Keccak algorithm.

const uint64_t RC[] = {
	0x0000000000000001L, 0x0000000000008082L, 0x800000000000808aL,
	0x8000000080008000L, 0x000000000000808bL, 0x0000000080000001L,
	0x8000000080008081L, 0x8000000000008009L, 0x000000000000008aL,
	0x0000000000000088L, 0x0000000080008009L, 0x000000008000000aL,
	0x000000008000808bL, 0x800000000000008bL, 0x8000000000008089L,
	0x8000000000008003L, 0x8000000000008002L, 0x8000000000000080L,
	0x000000000000800aL, 0x800000008000000aL, 0x8000000080008081L,
	0x8000000000008080L, 0x0000000080000001L, 0x8000000080008008L
};

const int R[] = {
	0, 1, 62, 28, 27, 36, 44, 6, 55, 20, 3, 10, 43,
	25, 39, 41, 45, 15, 21, 8, 18, 2, 61, 56, 14
};


inline int index(int x);
inline int index(int x, int y);

// Function to create the state structure for keccak application, of size length
//   (where length is the number of bits in the hash divided by 8. 
//   (eg 64 for SHA-3-512)
struct keccakState *keccakCreate(int length)
{
	struct keccakState *state = new keccakState;
	memset(state, 0, sizeof(keccakState));

	state->A = new uint64_t[25];
	memset(state->A, 0, 25*sizeof(uint64_t));
	state->blockLen = 200 - 2*(length/8);
	state->buffer = new uint8_t[state->blockLen];
	memset(state->buffer, 0, state->blockLen*sizeof(uint8_t));
	state->bufferLen = 0;
	state->length = length;
	state->d = length;
	return state;
}

// Function to create the state structure for SHAKE application, of size length
//   (where length is the number of bits in the hash divided by 8. 
//   (eg 32 for SHAKE256)
struct keccakState *shakeCreate(int length, unsigned int d_)
{
	struct keccakState *state = new keccakState;
	memset(state, 0, sizeof(keccakState));

	state->A = new uint64_t[25];
	memset(state->A, 0, 25 * sizeof(uint64_t));
	state->blockLen = 200 - 2 * (length / 8);
	state->buffer = new uint8_t[state->blockLen];
	memset(state->buffer, 0, state->blockLen*sizeof(uint8_t));
	state->bufferLen = 0;
	state->length = length;
	state->d = d_;
	return state;
}

void keccakReset(keccakState *state)
{
	for(unsigned int i = 0 ; i<25 ; i++) 
	{
		state->A[i] = 0L;
	}
	state->bufferLen = 0;
}

// keccakUpdate - Functions to pack input data into a block

//  One byte input at a time - process buffer if it's empty
void keccakUpdate(uint8_t input, keccakState *state)
{
	state->buffer[state->bufferLen] = input;
	if(++(state->bufferLen) == state->blockLen) 
	{
		keccakProcessBuffer(state);
	}
}

//  Process a larger buffer with varying amounts of data in it
void keccakUpdate(const uint8_t *input, int off, unsigned int len, keccakState *state)
{
	uint8_t *buffer = state->buffer;
	while (len > 0) 
	{
		int cpLen = 0;
		if((state->blockLen - state->bufferLen) > len)
		{
			cpLen = len;
		}
		else
		{
			cpLen = state->blockLen - state->bufferLen;
		}

		for(unsigned int i = 0 ; i!=cpLen ; i++)
		{
			buffer[state->bufferLen+i] = input[off+i];
		}
		state->bufferLen += cpLen;
		off += cpLen;
		len -= cpLen;
		if(state->bufferLen == state->blockLen) 
		{
			keccakProcessBuffer(state);
		}
	}
}


// keccakDigest - called once all data has been few to the keccakUpdate functions
//  Pads the structure (in case the input is not a multiple of the block length)
//  returns the hash result in a char array (not null terminated)
vector<unsigned char> keccakDigest(keccakState *state)
{
	uint64_t *A = state->A;
	unsigned int lengthInBytes = state->length / 8;
	unsigned int lengthInQuads = lengthInBytes / 8;

	keccakAddPadding(state);
	keccakProcessBuffer(state);
	vector<unsigned char> tmp;
	tmp.reserve(lengthInBytes);
	for(unsigned int i = 0 ; i < lengthInQuads ; i++) 
	{
		uint64_t b = NativeToLittle(A[i]);
		for (unsigned int j = 0 ; j!=8 ; j++)
		{
			tmp.push_back((unsigned char)((b >> (8*j)) & 0xFF));
		}
		//tmp[i] = NativeToLittle(A[i]);
	}
	keccakReset(state);
	return tmp;
}

// sha3Digest - called once all data has been few to the keccakUpdate functions
//  Pads the structure (in case the input is not a multiple of the block length)
//  returns the hash result in a char array (not null terminated)
vector<unsigned char> sha3Digest(keccakState *state)
{
	uint64_t *A = state->A;
	unsigned int lengthInBytes = state->length / 8;
	unsigned int lengthInQuads = lengthInBytes / 8;

	sha3AddPadding(state);
	keccakProcessBuffer(state);
	vector<unsigned char> tmp;
	tmp.reserve(lengthInBytes);
	for(unsigned int i = 0 ; i < lengthInQuads ; i++) 
	{
		uint64_t b = NativeToLittle(A[i]);
		for (unsigned int j = 0; j != 8; j++)
		{
			tmp.push_back((unsigned char)((b >> (8 * j)) & 0xFF));
		}
	}
	keccakReset(state);
	return tmp;
}

// shakeDigest - called once all data has been few to the keccakUpdate functions
//  Pads the structure (in case the input is not a multiple of the block length)
//  returns the hash result in a char array (not null terminated)
vector<unsigned char> shakeDigest(keccakState *state)
{
	uint64_t *A = state->A;
	unsigned int lengthInBytes = state->d / 8;
	unsigned int lengthInQuads = lengthInBytes / 64;

	shakeAddPadding(state);
	keccakProcessBuffer(state);
	vector<unsigned char> tmp;
	tmp.reserve(lengthInBytes);
	for (unsigned int i = 0 ; i < lengthInQuads ; i++)
	{
		uint64_t b = NativeToLittle(A[i]);
		for (unsigned int j = 0; j != 8; j++)
		{
			tmp.push_back((unsigned char)((b >> (8 * j)) & 0xFF));
		}

	}
	keccakReset(state);
	return tmp;
}

void sha3AddPadding(keccakState *state)
{
	unsigned int bufferLen = state->bufferLen;
	uint8_t *buffer = state->buffer;
	if(bufferLen + 1 == state->blockLen) 
	{
		buffer[bufferLen] = (uint8_t) 0x86;
	} 
	else 
	{
		buffer[bufferLen] = (uint8_t) 0x06;
		for(unsigned int i = bufferLen + 1 ; i < state->blockLen - 1 ; i++) 
		{
			buffer[i] = 0;
		}
		buffer[state->blockLen - 1] = (uint8_t) 0x80;
	}
}

void keccakAddPadding(keccakState *state)
{
	unsigned int bufferLen = state->bufferLen;
	uint8_t *buffer = state->buffer;
	if(bufferLen + 1 == state->blockLen) 
	{
		buffer[bufferLen] = (uint8_t) 0x81;
	} 
	else 
	{
		buffer[bufferLen] = (uint8_t) 0x01;
		for(unsigned int i = bufferLen + 1 ; i < state->blockLen - 1 ; i++) 
		{
			buffer[i] = 0;
		}
		buffer[state->blockLen - 1] = (uint8_t) 0x80;
	}
}

void shakeAddPadding(keccakState *state)
{
	unsigned int bufferLen = state->bufferLen;
	uint8_t *buffer = state->buffer;
	if (bufferLen + 1 == state->blockLen)
	{
		buffer[bufferLen] = (uint8_t)0x9F;
	}
	else
	{
		buffer[bufferLen] = (uint8_t)0x1F;
		for (unsigned int i = bufferLen + 1; i < state->blockLen - 1; i++)
		{
			buffer[i] = 0;
		}
		buffer[state->blockLen - 1] = (uint8_t)0x80;
	}
}

void keccakProcessBuffer(struct keccakState *state)
{
	uint64_t *A = state->A;
	for(unsigned int i = 0 ; i < state->blockLen/8 ; i++) 
	{
		A[i] ^= LittleToNative(((uint64_t*)state->buffer)[i]);
	}
	keccakf(state);
	state->bufferLen = 0;
}

inline int index(int x)
{
	return x < 0 ? index(x + 5) : x % 5;
}

inline int index(int x, int y)
{
	return index(x) + 5 * index(y);
}

struct keccakfState
{
	uint64_t B[25];
	uint64_t C[5];
	uint64_t D[5];
};


// Hash function proper. 
void keccakf(keccakState *state)
{
	uint64_t *A = state->A;
	keccakfState kState;
	for(int n = 0 ; n < 24 ; n++) 
	{
		for(int x = 0 ; x < 5 ; x++) 
		{
			kState.C[x] = A[index(x, 0)] ^ A[index(x, 1)] ^ A[index(x, 2)] ^ A[index(x, 3)] ^ A[index(x, 4)];
		}
		int i;
		int x = 0;
		int y = 0;
		x = 0; 
		kState.D[x] = kState.C[index(x - 1)] ^ rotateLeft(kState.C[index(x + 1)], 1);
		y = 0;
		A[index(x, y)] ^= kState.D[x];
		y = 1;
		A[index(x, y)] ^= kState.D[x];
		y = 2;
		A[index(x, y)] ^= kState.D[x];
		y = 3;
		A[index(x, y)] ^= kState.D[x];
		y = 4;
		A[index(x, y)] ^= kState.D[x];
		x = 1; 
		kState.D[x] = kState.C[index(x - 1)] ^ rotateLeft(kState.C[index(x + 1)], 1);
		y = 0;
		A[index(x, y)] ^= kState.D[x];
		y = 1;
		A[index(x, y)] ^= kState.D[x];
		y = 2;
		A[index(x, y)] ^= kState.D[x];
		y = 3;
		A[index(x, y)] ^= kState.D[x];
		y = 4;
		A[index(x, y)] ^= kState.D[x];
		x = 2; 
		kState.D[x] = kState.C[index(x - 1)] ^ rotateLeft(kState.C[index(x + 1)], 1);
		y = 0;
		A[index(x, y)] ^= kState.D[x];
		y = 1;
		A[index(x, y)] ^= kState.D[x];
		y = 2;
		A[index(x, y)] ^= kState.D[x];
		y = 3;
		A[index(x, y)] ^= kState.D[x];
		y = 4;
		A[index(x, y)] ^= kState.D[x];
		x = 3; 
		kState.D[x] = kState.C[index(x - 1)] ^ rotateLeft(kState.C[index(x + 1)], 1);
		y = 0;
		A[index(x, y)] ^= kState.D[x];
		y = 1;
		A[index(x, y)] ^= kState.D[x];
		y = 2;
		A[index(x, y)] ^= kState.D[x];
		y = 3;
		A[index(x, y)] ^= kState.D[x];
		y = 4;
		A[index(x, y)] ^= kState.D[x];
		x = 4; 
		kState.D[x] = kState.C[index(x - 1)] ^ rotateLeft(kState.C[index(x + 1)], 1);
		y = 0;
		A[index(x, y)] ^= kState.D[x];
		y = 1;
		A[index(x, y)] ^= kState.D[x];
		y = 2;
		A[index(x, y)] ^= kState.D[x];
		y = 3;
		A[index(x, y)] ^= kState.D[x];
		y = 4;
		A[index(x, y)] ^= kState.D[x];


		x = 0;
		y = 0;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 1;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 2;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 3;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 4;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		x = 1;
		y = 0;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 1;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 2;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 3;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 4;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);				
		x = 2;
		y = 0;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 1;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 2;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 3;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 4;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);			
		x = 3;
		y = 0;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 1;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 2;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 3;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 4;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);			
		x = 4;
		y = 0;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 1;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 2;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 3;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);
		y = 4;
		i = index(x, y);
		kState.B[index(y, x * 2 + 3 * y)] = rotateLeft(A[i], R[i]);			
				
		x = 0;
		y = 0;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 1;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 2;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 3;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 4;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		x = 1;
		y = 0;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 1;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 2;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 3;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 4;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		x = 2;
		y = 0;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 1;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 2;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 3;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 4;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		x = 3;
		y = 0;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 1;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 2;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 3;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 4;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		x = 4;
		y = 0;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 1;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 2;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 3;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		y = 4;
		i = index(x, y);
		A[i] = kState.B[i] ^ (~kState.B[index(x + 1, y)] & kState.B[index(x + 2, y)]);
		
		A[0] ^= RC[n];
	}
}

int main(int argc, char* argv[])
{
	parseCommandLine(argc, argv);


	return 0;
}

