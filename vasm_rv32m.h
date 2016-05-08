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

#endif
