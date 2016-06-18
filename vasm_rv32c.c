#include <ctype.h>
#include "vasm.h"

#include "vasm_rv32c.h"

#define SYM_INSTR_EXT "32c"

symbol_t sym_instr_rv32c[] =
{
    // FORMAT_CR
    SYM_INSTR_NAME_X(c_add, "c.add",
		     FORMAT_CR_CODE(OPCODE_C2, FUNCT_C_ADD),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ2(ASM_REG_RD, ASM_REG_RS2)),

    SYM_INSTR_NAME_X(c_ebreak, "c.ebreak",
		     FORMAT_CR_CODE(OPCODE_C2,FUNCT_C_EBREAK),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_jalr, "c.jalr",
		     FORMAT_CR_CODE(OPCODE_C2, FUNCT_C_JALR),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ2(ASM_RD_X1, ASM_REG_RS2)),

    SYM_INSTR_NAME_X(c_jr, "c.jr",
		     FORMAT_CR_CODE(OPCODE_C2, FUNCT_C_JR),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ2(ASM_RD_X0, ASM_REG_RS2)),

    SYM_INSTR_NAME_X(c_mv, "c.mv",
		     FORMAT_CR_CODE(OPCODE_C2, FUNCT_C_MV),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ2(ASM_REG_RD, ASM_REG_RS2)),

    SYM_INSTR_NAME_X(c_nop, "c.nop",
		     FORMAT_CR_CODE(OPCODE_C1, FUNCT_C_NOP),
		     FORMAT_CR, FORMAT_CR_MASK,
		     ASM_SEQ0()),

    // FORMAT_CI
    SYM_INSTR_NAME_X(c_addi, "c.addi",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_ADDI),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ2(ASM_REG_RD,  ASM_IMM_6)),

    SYM_INSTR_NAME_X(c_addi16sp, "c.addi16sp",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_ADDI16SP),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ2(ASM_REG_RD,  ASM_IMM_6)),

    SYM_INSTR_NAME_X(c_addiw, "c.addiw",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_ADDIW),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ2(ASM_REG_RD,  ASM_IMM_6)),

    SYM_INSTR_NAME_X(c_andi, "c.andi",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_ANDI)|0x00800000,
		     FORMAT_CI, FORMAT_CI_MASK|0x00c00000,
		     ASM_SEQ2(ASM_REG_CRD,ASM_IMM_6)),

    // FIXME 
    SYM_INSTR_NAME_X(c_fldsp, "c.fldsp",
		     FORMAT_CI_CODE(OPCODE_C2, FUNCT_C_FLDSP),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_flwsp, "c.flwsp",
		     FORMAT_CI_CODE(OPCODE_C2, FUNCT_C_FLWSP),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_ldsp, "c.ldsp",
		     FORMAT_CI_CODE(OPCODE_C2, FUNCT_C_LDSP),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_li, "c.li",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_LI),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_lui, "c.lui",
		     FORMAT_CI_CODE(OPCODE_C1, FUNCT_C_LUI),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_lwsp, "c.lwsp",
		     FORMAT_CI_CODE(OPCODE_C2, FUNCT_C_LWSP),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_slli, "c.slli",
		     FORMAT_CI_CODE(OPCODE_C2, FUNCT_C_SLLI),
		     FORMAT_CI, FORMAT_CI_MASK,
		     ASM_SEQ0()),

    // FORMAT_CSS
    SYM_INSTR_NAME_X(c_fsdsp, "c.fsdsp",
		     FORMAT_CSS_CODE(OPCODE_C2, FUNCT_C_FSDSP),
		     FORMAT_CSS, FORMAT_CSS_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_fswsp, "c.fswsp",
		     FORMAT_CSS_CODE(OPCODE_C2, FUNCT_C_FSWSP),
		     FORMAT_CSS, FORMAT_CSS_MASK,		     
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_sdsp, "c.sdsp",
		     FORMAT_CSS_CODE(OPCODE_C2, FUNCT_C_SDSP),
		     FORMAT_CSS, FORMAT_CSS_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_swsp, "c.swsp",
		     FORMAT_CSS_CODE(OPCODE_C2, FUNCT_C_SWSP),
		     FORMAT_CSS, FORMAT_CSS_MASK,
		     ASM_SEQ0()),

    // FORMAT_CIW    
    SYM_INSTR_NAME_X(c_addi4spn, "c.addi4spn",
		     FORMAT_CIW_CODE(OPCODE_C0, FUNCT_C_ADDI4SPN),
		     FORMAT_CIW, FORMAT_CIW_MASK,
		     ASM_SEQ0()),

    // FORMAT_CL
    SYM_INSTR_NAME_X(c_fld, "c.fld",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_FLD),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_flw, "c.flw",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_FLW),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_fsd, "c.fsd",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_FSD),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_fsw, "c.fsw",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_FSW),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_lw, "c.lw",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_LW),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_ld, "c.ld",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_LD),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_sd, "c.sd",
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_SD),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),
    SYM_INSTR_NAME_X(c_sw, "c.sw", 
		     FORMAT_CL_CODE(OPCODE_C0, FUNCT_C_SW),
		     FORMAT_CL, FORMAT_CL_MASK,
		     ASM_SEQ0()),

    // FORMAT_CS
    SYM_INSTR_NAME_X(c_sub, "c.sub",
		     FORMAT_CS_CODE(OPCODE_C1, FUNCT_C_SUB)|0b0000110000000000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ3(ASM_CONST_0, ASM_REG_CRD, ASM_REG_CRS2)),

    SYM_INSTR_NAME_X(c_xor, "c.xor",
		     FORMAT_CS_CODE(OPCODE_C1, FUNCT_C_XOR)|0b0000110000100000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_or, "c.or",
		     FORMAT_CS_CODE(OPCODE_C1,FUNCT_C_OR)|0b0000110001000000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ3(ASM_CONST_0, ASM_REG_CRD, ASM_REG_CRS2)),

    SYM_INSTR_NAME_X(c_and, "c.and",
		     FORMAT_CS_CODE(OPCODE_C1,FUNCT_C_AND)|0b0000110001100000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ3(ASM_CONST_0, ASM_REG_CRD, ASM_REG_CRS2)),

    SYM_INSTR_NAME_X(c_subw, "c.subw",
		     FORMAT_CS_CODE(OPCODE_C1,FUNCT_C_SUBW)|0b0001110000000000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ3(ASM_CONST_1, ASM_REG_CRD, ASM_REG_CRS2)),

    SYM_INSTR_NAME_X(c_addw, "c.addw",
		     FORMAT_CS_CODE(OPCODE_C1,FUNCT_C_ADDW)|0b0001110000100000,
		     FORMAT_CS, FORMAT_CS_MASK|0x1c60,
		     ASM_SEQ3(ASM_CONST_1, ASM_REG_CRD, ASM_REG_CRS2)),

    // FORMAT_CB
    SYM_INSTR_NAME_X(c_beqz, "c.beqz",
		     FORMAT_CB_CODE(OPCODE_C1, FUNCT_C_BEQZ),
		     FORMAT_CB, FORMAT_CB_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_bnez, "c.bnez",
		     FORMAT_CB_CODE(OPCODE_C1, FUNCT_C_BNEZ),
		     FORMAT_CB, FORMAT_CB_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_srai, "c.srai",
		     FORMAT_CB_CODE(OPCODE_C1, FUNCT_C_SRAI),
		     FORMAT_CB, FORMAT_CB_MASK,
		     ASM_SEQ0()),

    SYM_INSTR_NAME_X(c_srli, "c.srli",
		     FORMAT_CB_CODE(OPCODE_C1, FUNCT_C_SRLI),
		     FORMAT_CB, FORMAT_CB_MASK,
		     ASM_SEQ0()),

    // FORMAT_CJ
    SYM_INSTR_NAME_X(c_j, "c.j", 
		     FORMAT_CJ_CODE(OPCODE_C1, FUNCT_C_J),
		     FORMAT_CJ, FORMAT_CJ_MASK,
		     ASM_SEQ0()),

};

int vasm_rv32c_table_load(vasm_ctx_t* ctx)
{
    DEBUGF(ctx, "rv32c table load\n");

    symbol_table_install_array(&ctx->symtab, &sym_instr_rv32c[0],
			       sizeof_array(sym_instr_rv32c));
    return 0;
}
