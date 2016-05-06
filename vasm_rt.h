#ifndef __VASM_RT_H__
#define __VASM_RT_H__

#include <stdlib.h>
#include <memory.h>

#include "vasm_types.h"

#define MEMORY_SIZE   4096   // 4k bytes

#if defined(RV64I)
typedef int64_t reg_t;
typedef uint64_t ureg_t;
typedef uint64_t unsigned_t;
typedef int64_t  signed_t;
#elif defined(RV32I)
typedef int32_t reg_t;
typedef uint32_t ureg_t;
typedef uint32_t unsigned_t;
typedef int32_t  signed_t;
#endif
#define word_size (sizeof(reg_t)*8)
#define word_mask (word_size-1)

#if defined(RV32E)
#define NUM_REGISTERS 16
#else
#define NUM_REGISTERS 32
#endif

typedef struct _vasm_rt_t {
    reg_t     reg[NUM_REGISTERS];
    ureg_t    pc;
    size_t    mem_size;
    uint8_t   mem[MEMORY_SIZE];
    uint32_t  waddr;
} vasm_rt_t;

static register_t inline vasm_src(register_t y, int32_t z)
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
