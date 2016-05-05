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
#define FUNCT_SYS     0    // imm11_0 = 000000000000
#define FUNCT_SBREAK  0   // imm11_0 = 000000000001
#define FUNCT_CCSRD   2   // imm11_0 = 1100 0000 0000
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

// Instruction symbol index
enum {
    INSTR_add_SI,
    INSTR_sub_SI,
    INSTR_sll_SI,
    INSTR_slt_SI,
    INSTR_sltu_SI,
    INSTR_xor_SI,
    INSTR_srl_SI,
    INSTR_sra_SI,
    INSTR_or_SI,
    INSTR_and_SI,
    INSTR_addi_SI,
    INSTR_slli_SI,
    INSTR_slti_SI,
    INSTR_sltiu_SI,
    INSTR_xori_SI,
    INSTR_srli_SI,
    INSTR_srai_SI,
    INSTR_ori_SI,
    INSTR_andi_SI,
    INSTR_lui_SI,
    INSTR_auipc_SI,
    INSTR_lb_SI,
    INSTR_lbu_SI,
    INSTR_lh_SI,
    INSTR_lhu_SI,
    INSTR_lw_SI,
    INSTR_sb_SI,
    INSTR_sh_SI,
    INSTR_sw_SI,
    INSTR_fence_SI,
    INSTR_fence_i_SI,
    INSTR_beq_SI,
    INSTR_bne_SI,
    INSTR_blt_SI,
    INSTR_bltu_SI,
    INSTR_bge_SI,
    INSTR_bgeu_SI,
    INSTR_jal_SI,
    INSTR_jalr_SI,
    INSTR_scall_SI,
    INSTR_sbreak_SI,
    INSTR_rdcycle_SI,
    INSTR_rdcycleh_SI,
    INSTR_rdtime_SI,
    INSTR_rdtimeh_SI,
    INSTR_rdinstret_SI,
    INSTR_rdinstreth_SI
};

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
