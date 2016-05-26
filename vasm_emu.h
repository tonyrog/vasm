//
// Instructions emulation utils
//
#ifndef __VASM_EMU_H__
#define __VASM_EMU_H__

#include "vasm_rt.h"

static inline reg_t rdr(vasm_rt_t* ctx, int r)
{
    return (r == 0) ? 0 : ctx->reg[r];
}

static inline void wrr(vasm_rt_t* ctx, int r, reg_t val)
{
    if (r != 0)
	ctx->reg[r] = val;
}

#endif
