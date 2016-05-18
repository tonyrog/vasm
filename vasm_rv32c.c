#include <ctype.h>
#include "vasm.h"

#include "vasm_rv32c.h"

symbol_t sym_instr_rv32c[] =
{
    SYM_INSTR_NAME_X(c_add, "c.add", OPCODE_C2, FUNCT_C_ADD, FORMAT_CR, 0,
		     ASM_SEQ2(ASM_REG_RD, ASM_REG_RS2)),
    SYM_INSTR_NAME_X(c_addi, "c.addi", OPCODE_C1, FUNCT_C_ADD, FORMAT_CI, 0,
		     ASM_SEQ2(ASM_REG_RD,  ASM_IMM_6)),
    SYM_INSTR_NAME_X(c_addi16sp, "c.addi16sp", OPCODE_C1, FUNCT_C_ADD, FORMAT_CI, 0,
		     ASM_SEQ2(ASM_REG_RD,  ASM_IMM_6)),
};

int vasm_rv32c_asm_init(symbol_table_t* symtab)
{
    // link in static symbols above into symtab
    symbol_table_install_array(symtab, &sym_instr_rv32c[0],
			       sizeof_array(sym_instr_rv32c));
    return 0;
}
