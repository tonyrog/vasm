#ifndef __VASM_TYPES_H__
#define __VASM_TYPES_H__

#include <stdint.h>

typedef uint32_t unsigned_t;
typedef  int32_t signed_t;

#define word_size (sizeof(unsigned_t)*8)
#define word_mask (word_size-1)

#endif
