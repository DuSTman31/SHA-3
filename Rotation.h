#ifndef ROTATION_H_
#define ROTATION_H_

#include <cstdint>

// As we're not using assembly, we can't use the native rotation instructions
//  replace it with a small inline
static inline uint64_t rotateLeft(uint64_t x, int n)
{
	return ((x << n) | (x >> (64 - n)));
}

static inline uint64_t rotateRight(uint64_t x, int n)
{
	return ((x >> n) | (x << (64 - n)));
}

#endif //ROTATION_H_

