#include <ctype.h>
#include "vasm.h"

#include "vasm_rv32m.h"

symbol_t sym_instr_rv32m[] =
{
    SYM_INSTR_X(mul, OPCODE_ARITH, (0x01<<3)|FUNCT_MUL, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(mulh, OPCODE_ARITH, (0x01<<3)|FUNCT_MULH, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(mulhsu, OPCODE_ARITH, (0x01<<3)|FUNCT_MULHSU, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(mulhu, OPCODE_ARITH, (0x01<<3)|FUNCT_MULHU, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(div, OPCODE_ARITH, (0x01<<3)|FUNCT_DIV, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(divu, OPCODE_ARITH, (0x01<<3)|FUNCT_DIVU, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(rem, OPCODE_ARITH, (0x01<<3)|FUNCT_REM, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(remu, OPCODE_ARITH, (0x01<<3)|FUNCT_REMU, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
};

int vasm_rv32m_asm_init(symbol_table_t* symtab)
{
    symbol_table_install_array(symtab, &sym_instr_rv32m[0],
			       sizeof_array(sym_instr_rv32m));
    return 0;
}
