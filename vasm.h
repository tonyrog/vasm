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
} vasm_ctx_t;


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

// add,sub,sll,srl,sra,and,or,xor,slt,sltu
#define OPCODE_ARITH 0x33
#define FUNCT_ADD  0   // funct7 = 0000000
#define FUNCT_SUB  0   // funct7 = 0100000
#define FUNCT_SLL  1
#define FUNCT_SLT  2
#define FUNCT_SLTU 3
#define FUNCT_XOR  4
#define FUNCT_SRL  5   // funct7 = 0000000
#define FUNCT_SRA  5   // funct7 = 0100000
#define FUNCT_OR   6
#define FUNCT_AND  7

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
#define OPCODE_IMM   0x13
#define FUNCT_ADDI   0
#define FUNCT_SLLI   1
#define FUNCT_SLTI   2
#define FUNCT_SLTIU  3
#define FUNCT_XORI   4
#define FUNCT_SRLI   5  // imm11_0 = 0000000, shamt:5
#define FUNCT_SRAI   5  // imm11_0 = 0100000, shamt:5
#define FUNCT_ORI    6
#define FUNCT_ANDI   7

// lb, lbu, lh, lhu, lw
#define OPCODE_LOAD    0x03
#define FUNCT_LB       0
#define FUNCT_LH       1
#define FUNCT_LW       2
#define FUNCT_LBU      4
#define FUNCT_LHU      5

// fence, fence.i
#define OPCODE_FENCE   0x0F
#define FUNCT_FENCE    0
#define FUNCT_FENCE_I  1

// jalr
#define OPCODE_JALR  0x67

// scall, sbreak, csrrw, csrrc, csrrs, csrrwi, csrrci, csrrso
#define OPCODE_SYS    0x73
#define FUNCT_ECALL   0   // imm11_0 = 000000000000
#define FUNCT_EBREAK  0   // imm11_0 = 000000000001

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
#define OPCODE_STORE 0x23
#define FUNCT_SB 0
#define FUNCT_SH 1
#define FUNCT_SW 2

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

// beq, bne, blt, bltu, bge, bgeu
#define OPCODE_BRANCH  0x63
#define FUNCT_BEQ      0
#define FUNCT_BNE      1
#define FUNCT_BLT      4
#define FUNCT_BGE      5
#define FUNCT_BLTU     6
#define FUNCT_BGEU     7

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

// lui, auipc
#define OPCODE_LUI   0x37
#define OPCODE_AUIPC 0x17

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
#define OPCODE_JAL 0x6F

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

// vasm_asm.c
int asm_operand(token_t* tokens, int i, int* mode, int* rp);
int assemble(vasm_ctx_t* ctx, token_t* tokens, size_t num_tokens);

// vasm_disasm.c
extern void disasm_operand(FILE* f, int mode, int reg);
extern void disasm_instr(FILE* f,symbol_table_t* symtab,unsigned addr,
			 void* mem);
extern void disasm(FILE* f, symbol_table_t* symtab,  void* mem, size_t n);

// vasm_emu.c
extern unsigned_t get_operand(vasm_rt_t* ctx, int m, int r);
extern void set_operand(vasm_rt_t* ctx, int m, int r, unsigned_t value);
extern unsigned_t emu(vasm_rt_t* ctx, unsigned_t addr, void* mem);
extern void dump_regs(FILE* f, vasm_rt_t* ctx);
extern void run(FILE* f, symbol_table_t* symtab, vasm_rt_t* ctx, 
		unsigned_t addr);
#endif
