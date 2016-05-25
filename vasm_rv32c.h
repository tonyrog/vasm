#ifndef __VASM_RV32C_H__
#define __VASM_RV32C_H__

#include "vasm_symbol.h"

// Instruction symbol index
enum {
    INSTR_c_add_SI,        // CR
    INSTR_c_addw_SI,       // CR
    INSTR_c_ebreak_SI,     // CR
    INSTR_c_jalr_SI,       // CR
    INSTR_c_jr_SI,         // CR
    INSTR_c_mv_SI,         // CR
    INSTR_c_nop_SI,        // CR

    INSTR_c_addi_SI,       // CI
    INSTR_c_addi16sp_SI,   // CI
    INSTR_c_addiw_SI,      // CI
    INSTR_c_andi_SI,       // CI
    INSTR_c_fldsp_SI,      // CI
    INSTR_c_flwsp_SI,      // CI
    INSTR_c_ldsp_SI,       // CI
    INSTR_c_li_SI,         // CI
    INSTR_c_lui_SI,        // CI
    INSTR_c_lwsp_SI,       // CI
    INSTR_c_slli_SI,       // CI

    INSTR_c_fsdsp_SI,      // CSS
    INSTR_c_fswsp_SI,      // CSS
    INSTR_c_sdsp_SI,       // CSS
    INSTR_c_swsp_SI,       // CSS

    INSTR_c_addi4spn_SI,   // CIW

    INSTR_c_fld_SI,        // CL
    INSTR_c_flw_SI,        // CL
    INSTR_c_fsd_SI,        // CL
    INSTR_c_fsw_SI,        // CL
    INSTR_c_ld_SI,         // CL
    INSTR_c_lw_SI,         // CL
    INSTR_c_sd_SI,         // CL
    INSTR_c_sw_SI,         // CL

    INSTR_c_and_SI,        // CS
    INSTR_c_or_SI,         // CS
    INSTR_c_sub_SI,        // CS
    INSTR_c_subw_SI,       // CS
    INSTR_c_xor_SI,        // CS

    INSTR_c_beqz_SI,       // CB
    INSTR_c_bnez_SI,       // CB
    INSTR_c_srai_SI,       // CB
    INSTR_c_srli_SI,       // CB

    INSTR_c_j_SI,          // CJ
};

// base instruction masks
#define FORMAT_CR_MASK   0xf003
#define FORMAT_CI_MASK   0xe003
#define FORMAT_CSS_MASK  0xe003
#define FORMAT_CIW_MASK  0xe003
#define FORMAT_CL_MASK   0xe003
#define FORMAT_CS_MASK   0xe003
#define FORMAT_CB_MASK   0xe003
#define FORMAT_CJ_MASK   0xe003

#define FORMAT_CR_CODE(op,f4) ((op)|((f4)<<12))
#define FORMAT_CI_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CSS_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CIW_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CL_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CS_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CB_CODE(op,f3) ((op)|((f3)<<13))
#define FORMAT_CJ_CODE(op,f3) ((op)|((f3)<<13))

// cr-format
#define OPCODE_C0  0x0
#define FUNCT_C_ADDI4SPN   0
#define FUNCT_C_FLD        1  // RV32/64
#define FUNCT_C_LQ         1  // RV128
#define FUNCT_C_LW         2  // 
#define FUNCT_C_FLW        3  // RV32
#define FUNCT_C_LD         3  // RV64/128
#define FUNCT_C_FSD        5  // RV32/64
#define FUNCT_C_SQ         5  // RV128
#define FUNCT_C_SW         6
#define FUNCT_C_FSW        7  // RV32
#define FUNCT_C_SD         7  // RV64/RV128

#define OPCODE_C1  0x1
#define FUNCT_C_NOP        0
#define FUNCT_C_ADDI       0
#define FUNCT_C_JAL        1  // RV32
#define FUNCT_C_ADDIW      1  // RV64/128
#define FUNCT_C_LI         2
#define FUNCT_C_ADDI16SP   3
#define FUNCT_C_LUI        3
#define FUNCT_C_SRLI       4  // RV32
#define FUNCT_C_SRLI64     4  // RV128; RV32/64
#define FUNCT_C_SRAI       4  // RV32
#define FUNCT_C_SRAI64     4  // RV128; RV32/64
#define FUNCT_C_ANDI       4
#define FUNCT_C_SUB        4
#define FUNCT_C_XOR        4
#define FUNCT_C_OR         4
#define FUNCT_C_AND        4
#define FUNCT_C_SUBW       4  // RV64/128
#define FUNCT_C_ADDW       4  // RV64/128
#define FUNCT_C_J          5
#define FUNCT_C_BEQZ       6
#define FUNCT_C_BNEZ       7

#define OPCODE_C2 0x2
#define FUNCT_C_SLLI       0  // RV32
#define FUNCT_C_SLLI64     0  // RV128/RV32/64
#define FUNCT_C_FLDSP      1  // RV32/64
#define FUNCT_C_LWSP       2
#define FUNCT_C_FLWSP      3  // RV32
#define FUNCT_C_LDSP       3  // RV64/128
#define FUNCT_C_JR         8
#define FUNCT_C_MV         8
#define FUNCT_C_EBREAK     9
#define FUNCT_C_JALR       9
#define FUNCT_C_ADD        9
#define FUNCT_C_FSDSP      5  // RV32/64
#define FUNCT_C_FSQSP      5  // RV128
#define FUNCT_C_SWSP       6  //
#define FUNCT_C_FSWSP      7  // RV32
#define FUNCT_C_SDSP       7  // RV64/128

extern int vasm_rv32c_asm_init(symbol_table_t* symtab);

#endif
