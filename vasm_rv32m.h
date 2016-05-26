#ifndef __VASM_RV32M_H__
#define __VASM_RV32M_H__

#include "vasm_symbol.h"

enum {
    INSTR_mul_SI,
    INSTR_mulh_SI,
    INSTR_mulhu_SI,
    INSTR_mulhsu_SI,
    INSTR_div_SI,
    INSTR_divu_SI,
    INSTR_rem_SI,
    INSTR_remu_SI
};

#define FUNCT7_MULDIV 0x01

// r-format ( OPCODE_ARITH )
#define FUNCT_MUL     0  // funct7 = 0000001
#define FUNCT_MULH    1  // funct7 = 0000001
#define FUNCT_MULHSU  2  // funct7 = 0000001
#define FUNCT_MULHU   3  // funct7 = 0000001
#define FUNCT_DIV     4  // funct7 = 0000001
#define FUNCT_DIVU    5  // funct7 = 0000001
#define FUNCT_REM     6  // funct7 = 0000001
#define FUNCT_REMU    7  // funct7 = 0000001

extern int vasm_rv32m_asm_init(symbol_table_t* symtab);

static inline void rv32m_mul(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) * rdr(ctx,rs2));
}

#define RV32M_MUL(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));	\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));	\
	rv32m_mul((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_mulh(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    dreg_t r = rdr(ctx,rs1);
    r *= rdr(ctx,rs2);
    wrr(ctx,rd, r>>XLEN);
}

#define RV32M_MULH(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));	\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));	\
	rv32m_mulh((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_mulhsu(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    dreg_t r = rdr(ctx,rs1);
    r *= (ureg_t)rdr(ctx,rs2);
    wrr(ctx,rd, r>>XLEN);
}

#define RV32M_MULHSU(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));	\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));	\
	rv32m_mulhsu((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_mulhu(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    udreg_t r = (ureg_t)rdr(ctx,rs1);
    r *= (ureg_t)rdr(ctx,rs2);
    wrr(ctx,rd, r>>XLEN);
}

#define RV32M_MULHU(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));	\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));	\
	rv32m_mulhu((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_div(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    reg_t divisor = rdr(ctx,rs2);
    if (divisor == 0)
	wrr(ctx,rd,-1);
    else {
	reg_t dividend = rdr(ctx,rs1);
	if (dividend == XMIN_SIGNED)
	    wrr(ctx,rd,dividend);
	else
	    wrr(ctx,rd,dividend / divisor);
    }
}

#define RV32M_DIV(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));	\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));	\
	rv32m_div((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_divu(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    ureg_t divisor = (ureg_t)rdr(ctx,rs2);
    if (divisor == 0)
	wrr(ctx,rd,-1);
    else {
	ureg_t dividend = (ureg_t) rdr(ctx,rs1);
	wrr(ctx,rd,dividend / divisor);
    }
}

#define RV32M_DIVU(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));		\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));		\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));		\
	rv32m_divu((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32m_rem(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    reg_t divisor = rdr(ctx,rs2);
    reg_t dividend = rdr(ctx,rs1);
    if (divisor == 0)
	wrr(ctx,rd,dividend);
    else {
	if (dividend == XMIN_SIGNED)
	    wrr(ctx,rd,0);
	else
	    wrr(ctx,rd,dividend % divisor);
    }
}

#define RV32M_REM(ctx,ins) do {					\
	int _rd = bitfield_fetch(instr_r,rd,(ins));		\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));		\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));		\
	rv32m_rem((ctx),_rd,_rs1,_rs2);				\
    } while(0)

static inline void rv32m_remu(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    ureg_t divisor = rdr(ctx,rs2);
    if (divisor == 0)
	wrr(ctx,rd,0);
    else {
	ureg_t dividend = rdr(ctx,rs1);
	wrr(ctx,rd,dividend % divisor);
    }
}

#define RV32M_REMU(ctx,ins) do {				\
	int _rd = bitfield_fetch(instr_r,rd,(ins));		\
	int _rs1 = bitfield_fetch(instr_r,rs1,(ins));		\
	int _rs2 = bitfield_fetch(instr_r,rs2,(ins));		\
	rv32m_remu((ctx),_rd,_rs1,_rs2);			\
    } while(0)

#endif
