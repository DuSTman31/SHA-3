#pragma once

#include "stdafx.h"

// State structure
struct keccakState
{
	uint64_t *A;
	unsigned int blockLen;
	uint8_t *buffer;
	unsigned int bufferLen;
	int length;
	unsigned int d;
};



void keccakProcessBuffer(struct keccakState *state);
void keccakUpdate(const uint8_t *input, int off, int len, keccakState *state);
unsigned char *keccakDigest(keccakState *state);
unsigned char *sha3Digest(keccakState *state);
unsigned char *shakeDigest(keccakState *state);
void keccakAddPadding(keccakState *state);
void keccakf(keccakState *state);
void sha3AddPadding(keccakState *state);
void shakeAddPadding(keccakState *state);
struct keccakState *keccakCreate(int length);
struct keccakState *shakeCreate(int length, unsigned int d_);