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

typedef struct _vasm_ctx_t {
    vasm_rt_t  rt;
    char       linebuf[1024];
    symbol_table_t symtab;
    char*      filename;
    int        lineno;
    int        debug;
    int        verbose;
} vasm_ctx_t;

// 5 bit parser directive 6 tokens allowed
enum {
    ASM_END,
    ASM_REG_RD,
    ASM_REG_RS1,
    ASM_REG_RS2,
    ASM_IMM_5,
    ASM_IMM_12,
    ASM_IMM_20,
    ASM_REL_12,
    ASM_REL_20,
    ASM_IMM_12_RS1,
    ASM_IMM_IORW,
    ASM_COPY_RD_RS1,
    ASM_CONST_1,
    ASM_CONST_MINUS_1,
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
    FORMAT_CB,
    FORMAT_CJ
};

#include "vasm_rv32i.h"
#if defined(RV32M)
#include "vasm_rv32m.h"
#endif

static inline int32_t sign_extend(int32_t imm, int n)
{
    return ((imm<<(32-n)) >> (32-n));
}

typedef union _instr_t {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned data:25;
#else	
	unsigned data:25;
	unsigned opcode:7;
#endif
    };
} instr_t;

// instruction length
#define OPCODE_IS_16BIT(op) (((op) & 0x3)  !=  0x3)
#define OPCODE_IS_32BIT(op) (((op) & 0x1f) != 0x1f)
#define OPCODE_IS_48BIT(op) (((op) & 0x3f) == 0x1f)
#define OPCODE_IS_64BIT(op) (((op) & 0x7f) == 0x3f)

// add,sub,sll,srl,sra,and,or,xor,slt,sltu
typedef union _instr_r {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned rd:5;
	unsigned funct3:3;
	unsigned rs1:5;
	unsigned rs2:5;
	unsigned funct7:7;
#else
	unsigned funct7:7;
	unsigned rs2:5;
	unsigned rs1:5;
	unsigned funct3:3;
	unsigned rd:5;
	unsigned opcode:7;
#endif
    };
} instr_r;

// addi, slti, sltiu, xori, ori, andi, slli, srli, srai,
// lb, lbu, lh, lhu, lw
// fence, fence.i
// jalr
// scall, sbreak, csrrw, csrrc, csrrs, csrrwi, csrrci, csrrso

typedef union _instr_i {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned rd:5;
	unsigned funct3:3;
	unsigned rs1:5;
	signed imm11_0:12;
#else
	signed imm11_0:12;
	unsigned rs1:5;
	unsigned funct3:3;
	unsigned rd:5;
	unsigned opcode:7;
#endif
    };
} instr_i;

// sb, sh, sw
typedef union _instr_s {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned imm4_0:5;
	unsigned funct3:3;
	unsigned rs1:5;
	unsigned rs2:5;
	unsigned imm11_5:7;
#else
	unsigned imm11_5:7;
	unsigned rs2:5;
	unsigned rs1:5;
	unsigned funct3:3;
	unsigned imm4_0:5;
	unsigned opcode:7;
#endif
    };
} instr_s;

static inline void set_imm_s(instr_s* instr, int imm12)
{
    instr->imm4_0  = imm12 & 0x1f;
    instr->imm11_5 = imm12 >> 5;
}

static inline int32_t get_imm_s(instr_s* instr)
{
    int32_t imm12 = ((instr->imm11_5 << 5) | (instr->imm4_0));
    return sign_extend(imm12, 12);
}

// beq, bne, blt, bltu, bge, bgeu
typedef union _instr_sb {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned imm11:1;
	unsigned imm4_1:4;
	unsigned funct3:3;
	unsigned rs1:5;
	unsigned rs2:5;
	unsigned imm10_5:6;
	unsigned imm12:1;
#else
	unsigned imm12:1;
	unsigned imm10_5:6;
	unsigned rs2:5;
	unsigned rs1:5;
	unsigned funct3:3;
	unsigned imm4_1:4;
	unsigned imm11:1;
	unsigned opcode:7;
#endif
    };
} instr_sb;

static inline void set_imm_sb(instr_sb* instr, int imm)
{
    instr->imm4_1  = (imm >> 1) & 0xf;
    instr->imm10_5 = (imm >> 5) & 0x3f;
    instr->imm11   = (imm >> 11) & 0x1;
    instr->imm12   = (imm >> 12) & 0x1;
}

static inline int32_t get_imm_sb(instr_sb* instr)
{
    int32_t imm13 = ((instr->imm12<<12) | 
		     (instr->imm11<<11) |
		     (instr->imm10_5<<5) |
		     (instr->imm4_1 << 1));
    return sign_extend(imm13, 13);
}

// lui, auipc

typedef union _instr_u {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned rd:5;
	unsigned imm31_12:20;
#else
	unsigned imm31_12:20;
	unsigned rd:5;
	unsigned opcode:7;
#endif
    };
} instr_u;

// jal
typedef union _instr_uj {
    uint32_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:7;
	unsigned rd:5;
	unsigned imm19_12:8;
	unsigned imm11:1;
	unsigned imm10_1:10;
	unsigned imm20:1;
#else
	unsigned imm20:1;
	unsigned imm10_1:10;
	unsigned imm11:1;
	unsigned imm19_12:8;
	unsigned rd:5;
	unsigned opcode:7;
#endif
    };
} instr_uj;

static inline void set_imm_uj(instr_uj* instr, int imm)
{
    instr->imm19_12 = (imm >> 12) & 0xff;
    instr->imm11    = (imm >> 11) & 1;
    instr->imm10_1  = (imm >> 1) & 0x3ff;
    instr->imm20    = (imm >> 20) & 0x1;
}

static inline int32_t get_imm_uj(instr_uj* instr)
{
    int32_t imm21 = ((instr->imm20 << 20) |
		     (instr->imm19_12 << 12) |
		     (instr->imm11 << 11) |
		     (instr->imm10_1 << 1));
    return sign_extend(imm21, 21);
}

#if defined(RV32C)

#define PACKED __attribute__ ((__packed__))
// compressed formats 
typedef union PACKED _instr_c {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned data:14;
#else	
	unsigned data:14;
	unsigned opcode:2;
#endif
    };
} instr_c;

typedef union PACKED _instr_cr {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned rs2:5;
	unsigned rd:5;
	unsigned funct4:4;
#else	
	unsigned funct4:4;
	unsigned rs1:5;
	unsigned rd:5;
	unsigned opcode:2;
#endif
    };
} instr_cr;

typedef union PACKED _instr_ci {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned imm4_0:5;
	unsigned rs1:5;
	unsigned imm5:1;
	unsigned funct3:3;
#else	
	unsigned funct3:3;
	unsigned imm5:1;
	unsigned rs1:5;
	unsigned imm4_0:5;
	unsigned opcode:2;
#endif
    };
} instr_ci;

typedef union PACKED _instr_css {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned rs2:5;
	unsigned imm5_0:6;
	unsigned funct3:3;
#else	
	unsigned funct3:3;
	unsigned imm5_0:6;
	unsigned rs2:5;
	unsigned opcode:2;
#endif
    };
} instr_css;

typedef union PACKED _instr_ciw {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned rd8:3;
	unsigned imm7_0:8;
	unsigned funct3:3;
#else
	unsigned funct3:3;
	unsigned imm7_0:8;
	unsigned rd8:3;
	unsigned opcode:2;
#endif
    };
} instr_ciw;

typedef union PACKED _instr_cl {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned cd:3;
	unsigned imm1_0:2;
	unsigned cs1:3;
	unsigned imm4_2:3;
	unsigned funct3:3;
#else
	unsigned funct3:3;
	unsigned imm4_2:3;
	unsigned cs1:3;
	unsigned imm1_0:2;
	unsigned cd:3;
	unsigned opcode:2;
#endif
    };
} instr_cl;

typedef union PACKED _instr_cb {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	unsigned offsl:5;
	unsigned cs1:3;
	unsigned offsh:3;
	unsigned funct3:3;
#else
	unsigned funct3:3;
	unsigned offsh:3;
	unsigned cs1:3;
	unsigned offsl:5;
	unsigned opcode:2;
#endif
    };
} instr_cb;

typedef union PACKED _instr_cj {
    uint16_t instruction;
    struct {
#if BYTE_ORDER == LITTLE_ENDIAN
	unsigned opcode:2;
	signed offs:11;
	unsigned funct3:3;
#else
	unsigned funct3:3;
	signed offs:11;
	unsigned opcode:2;
#endif
    };
} instr_cj;

#endif

// vasm_asm.c

extern void asm_init(symbol_table_t* symtab); 
extern char* register_abi_name(int r);
extern char* register_xi_name(int r);
extern int assemble(vasm_ctx_t* ctx, token_t* tokens, size_t num_tokens);

// vasm_disasm.c
extern unsigned_t disasm_instr(FILE* f,symbol_table_t* symtab,
			       unsigned_t addr, void* mem);
extern void disasm(FILE* f, symbol_table_t* symtab, void* mem, size_t n);

// vasm_emu.c
extern unsigned_t get_operand(vasm_rt_t* ctx, int m, int r);
extern void set_operand(vasm_rt_t* ctx, int m, int r, unsigned_t value);
extern unsigned_t emu(vasm_rt_t* ctx, unsigned_t addr, void* mem);
extern void dump_regs(FILE* f, vasm_rt_t* ctx);
extern void run(FILE* f, symbol_table_t* symtab, vasm_rt_t* ctx, 
		unsigned_t addr);
#endif
