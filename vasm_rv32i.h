#ifndef __VASM_RV32I_H__
#define __VASM_RV32I_H__

#include "vasm_symbol.h"

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
    INSTR_rdinstreth_SI,

    // pseudo instructions
    INSTR_nop_SI,
    INSTR_mv_SI,
    INSTR_seqz_SI,
    INSTR_snez_SI,
    INSTR_not_SI,
    INSTR_j_SI,
    INSTR_li_SI,
    INSTR_inc_SI,
    INSTR_dec_SI,
};

// r-format
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

// i-format
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

#define OPCODE_LOAD    0x03
#define FUNCT_LB       0
#define FUNCT_LH       1
#define FUNCT_LW       2
#define FUNCT_LBU      4
#define FUNCT_LHU      5

#define OPCODE_FENCE   0x0F
#define FUNCT_FENCE    0
#define FUNCT_FENCE_I  1

#define FENCE_I_BIT 0x8
#define FENCE_O_BIT 0x4
#define FENCE_R_BIT 0x2
#define FENCE_W_BIT 0x1

#define OPCODE_JALR  0x67
#define OPCODE_SYS    0x73
#define FUNCT_SYS     0    // imm11_0 = 000000000000
#define FUNCT_SBREAK  0    // imm11_0 = 000000000001
#define FUNCT_CCSRD   2    // imm11_0 = 1100 0000 0000

// s-format
#define OPCODE_STORE 0x23
#define FUNCT_SB 0
#define FUNCT_SH 1
#define FUNCT_SW 2

// sb-format
#define OPCODE_BRANCH  0x63
#define FUNCT_BEQ      0
#define FUNCT_BNE      1
#define FUNCT_BLT      4
#define FUNCT_BGE      5
#define FUNCT_BLTU     6
#define FUNCT_BGEU     7

// u-format
#define OPCODE_LUI   0x37
#define OPCODE_AUIPC 0x17

// uj-format
#define OPCODE_JAL 0x6F

extern int vasm_rv32i_asm_init(symbol_table_t* symtab);

#endif
