#ifndef __VASM_RT_H__
#define __VASM_RT_H__

#include <stdlib.h>
#include <memory.h>

#include "vasm_types.h"

#define NUM_REGISTERS 32
#define MEMORY_SIZE   1024

typedef struct _vasm_rt_t {
    uint32_t reg[NUM_REGISTERS];
    size_t mem_size;
    uint32_t mem[MEMORY_SIZE];
    uint32_t mem_addr;
} vasm_rt_t;

static uint32_t inline vasm_src(uint32_t y, int32_t z)
{
    if (z < 0) { // shift left circular
	z = -z;
	z = z & word_mask;  // 0..31
	return (y << z) | (y >> (word_size - z));
    }
    else {  // shift right circular
	z = z & word_mask;  // 0..31
	return (y >> z) | (y << (word_size - z));
    }    
}

// generate inline call and link
#define vasm_call(ctx, rd, label) do {		\
	__label__ lret;				\
	(ctx)->reg[(rd)] = (unsigned_t) &&lret;	\
	goto label;				\
    lret:					\
	;					\
    } while(0)

#define vasm_ret(ctx, rd) goto *((void*)((ctx)->reg[(rd)]))

#endif
