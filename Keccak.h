#pragma once

#include "stdafx.h"
#include <vector>
using std::vector;

// State structure
struct keccakState
{
	uint64_t *A;
	unsigned int blockLen;
	uint8_t *buffer;
	unsigned int bufferLen;
	unsigned int length;
	unsigned int d;
};



void keccakProcessBuffer(struct keccakState *state);
void keccakUpdate(const uint8_t *input, int off, unsigned int len, keccakState *state);
vector<unsigned char> keccakDigest(keccakState *state);
vector<unsigned char> sha3Digest(keccakState *state);
vector<unsigned char> shakeDigest(keccakState *state);
void keccakAddPadding(keccakState *state);
void keccakf(keccakState *state);
void sha3AddPadding(keccakState *state);
void shakeAddPadding(keccakState *state);
struct keccakState *keccakCreate(unsigned int length);
struct keccakState *shakeCreate(unsigned int length, unsigned int d_);