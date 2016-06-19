#ifndef __VASM_RV32F_H__
#define __VASM_RV32F_H__

#include "vasm_symbol.h"
#include "vasm_emu.h"

#define RNE  0x0
#define RTZ  0x1
#define RDN  0x2
#define RUP  0x3
#define RMM  0x4
#define RI1  0x5
#define RI2  0x6
#define RDY  0x7   // dynamic

// Instruction symbol index
enum {
    INSTR_flw_SI,
    INSTR_fsw_SI,
    INSTR_fmadd_SI,
    INSTR_fmsub_s_SI,
    INSTR_fnmsub_s_SI,
    INSTR_fnmadd_s_SI,
    INSTR_fadd_s_SI,
    INSTR_fsub_s_SI,
    INSTR_fmul_s_SI,
    INSTR_fdiv_s_SI,
    INSTR_fsqrt_s_SI,
    INSTR_fsgnj_s_SI,
    INSTR_fsgnjn_s_SI,
    INSTR_fsgnjx_s_SI,
    INSTR_fmin_s_SI,
    INSTR_fmax_s_SI,
    INSTR_fcvt_w_s_SI,
    INSTR_fcvt_wu_s_SI,
    INSTR_fmv_x_s_SI,
    INSTR_feq_s_SI,
    INSTR_flt_s_SI,
    INSTR_fle_s_SI,
    INSTR_fclass_s_SI,
    INSTR_fcvt_s_w_SI,
    INSTR_fcvt_s_wu_SI,
    INSTR_fmv_s_x_SI,
    INSTR_frcsr_SI,
    INSTR_frrm_SI,
    INSTR_frflags_SI,
    INSTR_fsrm_SI,
    INSTR_fsflags_SI,
    INSTR_fsrmi_SI,
    INSTR_fsflagsi_SI,
};

#define OPCODE_FLW 0x07
#define OPCODE_FSW 0x27

#define OPCODE_FMADD_S  0x43
#define OPCODE_FMSUB_S  0x47
#define OPCODE_FNMSUB_S 0x4B
#define OPCODE_FNMADD_S 0x4F

#define OPCODE_FARITH   0x53
#define FUNCT_FADD_S    0x00
#define FUNCT_FSUB_S    0x04
#define FUNCT_FMUL_S    0x08
#define FUNCT_DIV_S     0x0C
#define FUNCT_SQRT_S    0x2C  // frs2 = 0
#define FUNCT_SGN_S     0x10
#define FUNCT3_SGNJ_S     0x0
#define FUNCT3_SGNJN_S    0x1
#define FUNCT3_SGNJX_S    0x2
#define FUNCT_MINMAX_S  0x14
#define FUNCT3_MIN_S      0x0
#define FUNCT3_MAX_S      0x1
#define FUNCT_FCVT_S    0x60
#define RS2_FCVT_W_S    0x0   // RS2 = 0
#define RS2_FCVT_WH_S   0x1   // RS2 = 1
#define FUNCT_FMV_X_S   0x70  // RS2 = 0
#define FUNCT_CMP_S     0x50
#define FUNCT3_FEQ_S    0x2
#define FUNCT3_FLT_S    0x1
#define FUNCT3_FLE_S    0x0
#define FUNCT_FCLASS_S  0x70  // RS2 = 0
#define FUNCT_FCVT_W    0x64
#define RS2_FCVT_S_W    0x0
#define RS2_FCVT_S_WU   0x01
#define FUNCT_FMV_S_X   0x78  // RS2=0 FUNC3=0

#endif
