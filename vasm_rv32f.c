#include <ctype.h>
#include "vasm.h"

#include "vasm_rv32f.h"

#define SYMREG_FI(i, nm) \
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG|SYMBOL_FLAG_FREG, .index = (i), .name = (nm)  }

symbol_t sym_reg_fi[] =
{
    SYMREG_FI(0, "f0"),
    SYMREG_FI(1, "f1"),
    SYMREG_FI(2, "f2"),
    SYMREG_FI(3, "f3"),
    SYMREG_FI(4, "f4"),
    SYMREG_FI(5, "f5"),
    SYMREG_FI(6, "f6"),
    SYMREG_FI(7, "f7"),
    SYMREG_FI(8, "f8"),
    SYMREG_FI(9, "f9"),
    SYMREG_FI(10, "f10"),
    SYMREG_FI(11, "f11"),
    SYMREG_FI(12, "f12"),
    SYMREG_FI(13, "f13"),
    SYMREG_FI(14, "f14"),
    SYMREG_FI(15, "f15"),
    SYMREG_FI(16, "f16"),
    SYMREG_FI(17, "f17"),
    SYMREG_FI(18, "f18"),
    SYMREG_FI(19, "f19"),
    SYMREG_FI(20, "f20"),
    SYMREG_FI(21, "f21"),
    SYMREG_FI(22, "f22"),
    SYMREG_FI(23, "f23"),
    SYMREG_FI(24, "f24"),
    SYMREG_FI(25, "f25"),
    SYMREG_FI(26, "f26"),
    SYMREG_FI(27, "f27"),
    SYMREG_FI(28, "f28"),
    SYMREG_FI(29, "f29"),
    SYMREG_FI(30, "f30"),
    SYMREG_FI(31, "f31"),
};

char* register_fi_name(int r)
{
    return sym_reg_fi[r].name;
}

#define SYMREG_FABI(i, nm) \
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG|SYMBOL_FLAG_FREG,\
	      .index = (i), .name = (nm)  }

#define SYMREG_FABI_IX(i,ix,nm)						\
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG|SYMBOL_FLAG_FREG, .index = (ix), .name = (nm)  }

symbol_t sym_reg_fabi[] = 
{
    SYMREG_FABI(0, "ft0"),
    SYMREG_FABI(1, "ft1"),
    SYMREG_FABI(2, "ft2"),
    SYMREG_FABI(3, "ft3"),
    SYMREG_FABI(4, "ft4"),
    SYMREG_FABI(5, "ft5"),
    SYMREG_FABI(6, "ft6"),
    SYMREG_FABI(7, "ft7"),
    SYMREG_FABI(8, "fs0"),
    SYMREG_FABI(9, "fs1"),
    SYMREG_FABI(10, "fa0"),
    SYMREG_FABI(11, "fa1"),
    SYMREG_FABI(12, "fa2"),
    SYMREG_FABI(13, "fa3"),
    SYMREG_FABI(14, "fa4"),
    SYMREG_FABI(15, "fa5"),
    SYMREG_FABI(16, "fa6"),
    SYMREG_FABI(17, "fa7"),
    SYMREG_FABI(18, "fs2"),
    SYMREG_FABI(19, "fs3"),
    SYMREG_FABI(20, "fs4"),
    SYMREG_FABI(21, "fs5"),
    SYMREG_FABI(22, "fs6"),
    SYMREG_FABI(23, "fs7"),
    SYMREG_FABI(24, "fs8"),
    SYMREG_FABI(25, "fs9"),
    SYMREG_FABI(26, "fs10"),
    SYMREG_FABI(27, "fs11"),
    SYMREG_FABI(28, "ft8"),
    SYMREG_FABI(29, "ft9"),
    SYMREG_FABI(30, "ft10"),
    SYMREG_FABI(31, "ft11"),
};

char* register_fabi_name(int r)
{
    return sym_reg_fabi[r].name;
}

#define SYM_INSTR_EXT "32f"

symbol_t sym_instr_rv32f[] =
{
    // What is the instruction names for rounding modes? and dynamic?
    SYM_INSTR_NAME_X(fadd_s, "fadd.s", 
		     FORMAT_R_CODE(OPCODE_FARITH,RNE,FUNCT_FADD_S),
		     FORMAT_R, FORMAT_R_MASK,
		     ASM_SEQ3(ASM_REG_FRD, ASM_REG_FRS1, ASM_REG_FRS2)),
    SYM_INSTR_NAME_X(fsub_s, "fsub.s", 
		     FORMAT_R_CODE(OPCODE_FARITH,RNE,FUNCT_FSUB_S),
		     FORMAT_R, FORMAT_R_MASK,
		     ASM_SEQ3(ASM_REG_FRD, ASM_REG_FRS1, ASM_REG_FRS2)),


};

int vasm_rv32f_table_load(vasm_ctx_t* ctx)
{
    // link in static symbols above into symtab
    symbol_table_install_array(&ctx->symtab, &sym_reg_fabi[0], 
			       sizeof_array(sym_reg_fabi));
    symbol_table_install_array(&ctx->symtab, &sym_reg_fi[0],
			       sizeof_array(sym_reg_fi));
    symbol_table_install_array(&ctx->symtab, &sym_instr_rv32f[0],
			       sizeof_array(sym_instr_rv32f));
    return 0;
}
