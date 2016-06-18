#ifndef __VASM_H__
#define __VASM_H__

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <machine/endian.h>
#else
#include <endian.h>
#endif

#include "vasm_types.h"
#include "vasm_symbol.h"
#include "vasm_scan.h"
#include "vasm_rt.h"
#include "vasm_bits.h"

typedef struct _vasm_ctx_t {
    vasm_rt_t  rt;
    char       linebuf[1024];
    symbol_table_t symtab;
    char*      filename;
    int        lineno;
    int        debug;
    int        verbose;
} vasm_ctx_t;

#define DEBUGF(ctx, ...) do {			\
	if ((ctx)->debug) {			\
	    fprintf(stderr, __VA_ARGS__);	\
	}					\
    } while(0)

// 5 bit parser directive 6 tokens allowed
enum {
    ASM_END,
    ASM_REG_RD,
    ASM_REG_CRD,
    ASM_REG_RS1,
    ASM_REG_CRS1,
    ASM_REG_RS2,
    ASM_REG_CRS2,
    ASM_SHAMT_5,
    ASM_IMM_6,
    ASM_IMM_8,
    ASM_IMM_12,
    ASM_UIMM_20,
    ASM_REL_12,
    ASM_REL_20,
    ASM_IMM_12_RS1,
    ASM_IMM_IORW,
    ASM_COPY_RD_RS1,
    ASM_CONST_0,
    ASM_CONST_1,
    ASM_CONST_MINUS_1,
    ASM_RD_X0,
    ASM_RD_X1,
    // MAX 32!
};

#define ASM_SEQ6(x0,x1,x2,x3,x4,x5) \
    (((x0)<<0)|((x1)<<5)|((x2)<<10)|((x3)<<15)|((x4)<<20)|((x5)<<25))
#define ASM_SEQ5(x0,x1,x2,x3,x4)				\
    (((x0)<<0)|((x1)<<5)|((x2)<<10)|((x3)<<15)|((x4)<<20))
#define ASM_SEQ4(x0,x1,x2,x3)				\
    (((x0)<<0)|((x1)<<5)|((x2)<<10)|((x3)<<15))
#define ASM_SEQ3(x0,x1,x2)				\
    (((x0)<<0)|((x1)<<5)|((x2)<<10))
#define ASM_SEQ2(x0,x1)				\
    (((x0)<<0)|((x1)<<5))
#define ASM_SEQ1(x0)				\
    (((x0)<<0))
#define ASM_SEQ0()				\
    (0)

enum {
    FORMAT_R,
    FORMAT_I,
    FORMAT_S,
    FORMAT_SB,
    FORMAT_U,
    FORMAT_UJ,

    FORMAT_CR,
    FORMAT_CI,
    FORMAT_CSS,
    FORMAT_CIW,
    FORMAT_CL,
    FORMAT_CS,
    FORMAT_CB,
    FORMAT_CJ
};

#include "vasm_rv32i.h"
#if defined(RV32M)
#include "vasm_rv32m.h"
#endif
#if defined(RV32C)
#include "vasm_rv32c.h"
#endif

static inline int32_t sign_extend(int32_t imm, int n)
{
    return ((imm<<(32-n)) >> (32-n));
}

bit_struct(instr_t, {
	unsigned_field(opcode,7);
	unsigned_field(data,25);
    });

// instruction length
#define OPCODE_IS_16BIT(op) (((op) & 0x3)  !=  0x3)
#define OPCODE_IS_32BIT(op) (((op) & 0x1f) != 0x1f)
#define OPCODE_IS_48BIT(op) (((op) & 0x3f) == 0x1f)
#define OPCODE_IS_64BIT(op) (((op) & 0x7f) == 0x3f)

// add,sub,sll,srl,sra,and,or,xor,slt,sltu
bit_struct(instr_r,
	   {
	       unsigned_field(opcode, 7);
	       unsigned_field(rd, 5);
	       unsigned_field(funct3, 3);
	       unsigned_field(rs1, 5);
	       unsigned_field(rs2, 5);
	       unsigned_field(funct7, 7);
	   });

// addi, slti, sltiu, xori, ori, andi, slli, srli, srai,
// lb, lbu, lh, lhu, lw
// fence, fence.i
// jalr
// ecall, ebreak, csrrw, csrrc, csrrs, csrrwi, csrrci, csrrso

bit_struct(instr_i,
	   {
	       unsigned_field(opcode, 7);
	       unsigned_field(rd, 5);
	       unsigned_field(funct3, 3);
	       unsigned_field(rs1, 5);
	       signed_field(imm11_0, 12);
	   });

// sb, sh, sw
bit_struct(instr_s, {
	unsigned_field(opcode,7);
	unsigned_field(imm4_0,5);
	unsigned_field(funct3,3);
	unsigned_field(rs1,5);
	unsigned_field(rs2,5);
	unsigned_field(imm11_5,7);
    });

static inline uint32_t set_imm_s(uint32_t ins, int imm12)
{
    ins = bitfield_store(instr_s, imm4_0, ins, imm12 & 0x1f);
    ins = bitfield_store(instr_s, imm11_5, ins, imm12 >> 5);
    return ins;
}

static inline int32_t get_imm_s(uint32_t ins)
{
    int32_t imm12 = 
	(bitfield_fetch(instr_s, imm11_5, ins) << 5) |
	bitfield_fetch(instr_s, imm4_0, ins);
    return sign_extend(imm12, 12);
}

// beq, bne, blt, bltu, bge, bgeu
bit_struct(instr_sb, {
	unsigned_field(opcode,7);
	unsigned_field(imm11,1);
	unsigned_field(imm4_1,4);
	unsigned_field(funct3,3);
	unsigned_field(rs1,5);
	unsigned_field(rs2,5);
	unsigned_field(imm10_5,6);
	unsigned_field(imm12,1);
    });

static inline uint32_t set_imm_sb(uint32_t ins, int imm)
{
    ins = bitfield_store(instr_sb, imm4_1,ins, (imm>>1) & 0xf);
    ins = bitfield_store(instr_sb, imm10_5, ins, (imm >> 5) & 0x3f);
    ins = bitfield_store(instr_sb, imm11, ins, (imm >> 11) & 0x1);
    ins = bitfield_store(instr_sb, imm12, ins, (imm >> 12) & 0x1);
    return ins;
}

static inline int32_t get_imm_sb(uint32_t ins)
{
    int32_t imm13 = 
	(bitfield_fetch(instr_sb, imm12, ins) << 12) |
	(bitfield_fetch(instr_sb, imm11, ins) << 11) |
	(bitfield_fetch(instr_sb, imm10_5, ins) << 5) |
	(bitfield_fetch(instr_sb, imm4_1, ins) << 1);
    return sign_extend(imm13, 13);
}

// lui, auipc

bit_struct(instr_u, {
	unsigned_field(opcode,7);
	unsigned_field(rd,5);
	unsigned_field(imm31_12,20);
    });

// jal
bit_struct(instr_uj, {
	unsigned_field(opcode,7);
	unsigned_field(rd,5);
	unsigned_field(imm19_12,8);
	unsigned_field(imm11,1);
	unsigned_field(imm10_1,10);
	unsigned_field(imm20,1);
    });


static inline uint32_t set_imm_uj(uint32_t ins, int imm)
{
    ins = bitfield_store(instr_uj, imm19_12, ins,  (imm >> 12) & 0xff);
    ins = bitfield_store(instr_uj, imm11, ins,  (imm >> 11) & 1);
    ins = bitfield_store(instr_uj, imm10_1, ins,  (imm >> 1) & 0x3ff);
    ins = bitfield_store(instr_uj, imm20, ins,  (imm >> 20) & 1);
    return ins;
}

static inline int32_t get_imm_uj(uint32_t ins)
{
    int32_t imm21 = 
	(bitfield_fetch(instr_uj,imm20,ins) << 20) |
	(bitfield_fetch(instr_uj,imm19_12,ins) << 12) |
	(bitfield_fetch(instr_uj,imm11,ins) << 11) |
	(bitfield_fetch(instr_uj,imm10_1,ins) << 1);
    return sign_extend(imm21, 21);
}

// compressed formats 
bit_struct(instr_c, {
	unsigned_field(opcode,2);
	unsigned_field(data,11);
	unsigned_field(funct3,3);
    });

bit_struct(instr_cr, {
	unsigned_field(opcode,2);
	unsigned_field(rs2,5);
	unsigned_field(rd,5);
	unsigned_field(funct4,4);
    });

// Compressed Immediate
bit_struct(instr_ci, {
	unsigned_field(opcode,2);
	unsigned_field(imm6_2,5);
	unsigned_field(rd,5);
	unsigned_field(imm12,1);
	unsigned_field(funct3,3);
    });

// Compressed Stack-relative Store
bit_struct(instr_css, {
	unsigned_field(opcode,2);
	unsigned_field(rs2,5);
	unsigned_field(imm12_7,6);
	unsigned_field(funct3,3);
    });

// Wide Immediate
bit_struct(instr_ciw, {
	unsigned_field(opcode,2);
	unsigned_field(rd,3);
	unsigned_field(imm12_5,8);
	unsigned_field(funct3,3);
    });

// Compressed load
bit_struct(instr_cl, {
	unsigned_field(opcode,2);
	unsigned_field(rd,3);
	unsigned_field(imm6_5,2);
	unsigned_field(rs1,3);
	unsigned_field(imm12_10,3);
	unsigned_field(funct3,3);
    });

// Compressed store
bit_struct(instr_cs, {
	unsigned_field(opcode,2);
	unsigned_field(rs2,3);
	unsigned_field(imm6_5,2);
	unsigned_field(rd,3);
	unsigned_field(imm12_10,3);
	unsigned_field(funct3,3);
    });

// Compressed Branch
bit_struct(instr_cb, {
	unsigned_field(opcode,2);
	unsigned_field(imm6_2,5);
	unsigned_field(rs1,3);
	unsigned_field(imm12_10,3);
	unsigned_field(funct3,3);
    });

// Compressed Jump
bit_struct(instr_cj, {
	unsigned_field(opcode,2);
	signed_field(imm12_2,11);
	unsigned_field(funct3,3);
    });

#define FORMAT_R_MASK  0xFE00707f
#define FORMAT_I_MASK  0x0000707f
#define FORMAT_S_MASK  0x0000707f
#define FORMAT_SB_MASK 0x0000707f
#define FORMAT_U_MASK  0x0000007f
#define FORMAT_UJ_MASK 0x0000007f

#define FORMAT_R_CODE(op,f3,f7) ((op)|((f3)<<12)|((f7)<<25))
#define FORMAT_I_CODE(op,f3)    ((op)|((f3)<<12))
#define FORMAT_S_CODE(op,f3)    ((op)|((f3)<<12))
#define FORMAT_SB_CODE(op,f3)   ((op)|((f3)<<12))
#define FORMAT_U_CODE(op)       ((op))
#define FORMAT_UJ_CODE(op)      ((op))

#define RV32I_DECODE_R					\
    int _rd = bitfield_fetch(instr_r,rd,(ins));		\
    int _rs1 = bitfield_fetch(instr_r,rs1,(ins));	\
    int _rs2 = bitfield_fetch(instr_r,rs2,(ins))

#define RV32I_DECODE_I					\
    int _rd = bitfield_fetch(instr_i,rd,(ins));		\
    int _rs1 = bitfield_fetch(instr_i,rs1,(ins));		\
    int _imm = bitfield_fetch_signed(instr_i,imm11_0,(ins))

#define RV32I_DECODE_S						\
    int _rs1 = bitfield_fetch(instr_s,rs1,(ins));		\
    int _rs2 = bitfield_fetch(instr_s,rs2,(ins));		\
    int _imm = get_imm_s((ins))

#define RV32I_DECODE_SB						\
    int _rs1 = bitfield_fetch(instr_sb,rs1,(ins));		\
    int _rs2 = bitfield_fetch(instr_sb,rs2,(ins));		\
    int _imm = get_imm_sb((ins))

#define RV32I_DECODE_U					\
    int _rd = bitfield_fetch(instr_u,rd,(ins));		\
    int _imm = bitfield_fetch(instr_u,imm31_12,ins)

#define RV32I_DECODE_UJ						\
    int _rd = bitfield_fetch(instr_uj,rd,(ins));		\
    int _imm = get_imm_uj((ins))

// vasm_asm.c

extern char* register_abi_name(int r);
extern char* register_xi_name(int r);
extern int assemble(vasm_ctx_t* ctx, token_t* tokens, size_t num_tokens);

// vasm_disasm.c
extern unsigned_t disasm_instr(FILE* f,symbol_table_t* symtab,
			       unsigned_t addr, void* mem);
extern void disasm(FILE* f, symbol_table_t* symtab, void* mem, size_t n);

// vasm_emu.c
extern unsigned_t emu(vasm_rt_t* ctx, unsigned_t addr, void* mem);
extern void dump_regs(FILE* f, vasm_rt_t* ctx);
extern void run(FILE* f, symbol_table_t* symtab, vasm_rt_t* ctx, 
		unsigned_t addr);

// vasm.c
extern void vasm_init(vasm_ctx_t* ctx);

#endif
