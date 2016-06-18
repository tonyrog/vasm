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

#define OPCODE_FLW 0x7

#endif
