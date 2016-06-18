#ifndef __VASM_RV32I_H__
#define __VASM_RV32I_H__

#include "vasm_emu.h"
#include "vasm.h"

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
    INSTR_ecall_SI,
    INSTR_ebreak_SI,
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

extern int vasm_rv32i_table_load(vasm_ctx_t* ctx);

// used by emulator and C-code generation

static inline void rv32i_add(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) + rdr(ctx,rs2));
}

#define RV32I_ADD(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_add((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_sub(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) - rdr(ctx,rs2));
}

#define RV32I_SUB(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_sub((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_sll(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) << rdr(ctx,rs2));
}

#define RV32I_SLL(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_sll((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_sra(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) >> rdr(ctx,rs2));
}

#define RV32I_SRA(ctx,ins) do {			\
	RV32I_DECODE_R;					\
	rv32i_sra((ctx),_rd,_rs1,_rs2);			\
    } while(0)


static inline void rv32i_slt(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd, (rdr(ctx,rs1) < rdr(ctx,rs2)));
}

#define RV32I_SLT(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_slt((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_sltu(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd, ((ureg_t)rdr(ctx,rs1) < (ureg_t)rdr(ctx,rs2)));
}

#define RV32I_SLTU(ctx,ins) do {			\
	RV32I_DECODE_R;					\
	rv32i_sltu((ctx),_rd,_rs1,_rs2);		\
    } while(0)

static inline void rv32i_xor(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) ^ rdr(ctx,rs2));
}

#define RV32I_XOR(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_xor((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_srl(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,(ureg_t)rdr(ctx,rs1) >> rdr(ctx,rs2));
}

#define RV32I_SRL(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_srl((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_or(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) | rdr(ctx,rs2));
}

#define RV32I_OR(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_or((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_and(vasm_rt_t* ctx, int rd, int rs1, int rs2)
{
    wrr(ctx,rd,rdr(ctx,rs1) & rdr(ctx,rs2));
}

#define RV32I_AND(ctx,ins) do {				\
	RV32I_DECODE_R;					\
	rv32i_and((ctx),_rd,_rs1,_rs2);			\
    } while(0)

static inline void rv32i_addi(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,rdr(ctx,rs1) + imm);
}

#define RV32I_ADDI(ctx,ins) do {		\
	RV32I_DECODE_I;				\
	rv32i_addi((ctx),_rd,_rs1,_imm);	\
    } while(0)

static inline void rv32i_slti(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,(rdr(ctx,rs1) < imm));
}

#define RV32I_SLTI(ctx,ins) do {		\
	RV32I_DECODE_I;				\
	rv32i_slti((ctx),_rd,_rs1,_imm);	\
    } while(0)

static inline void rv32i_sltiu(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,((ureg_t)rdr(ctx,rs1) < (ureg_t)imm));
}

#define RV32I_SLTIU(ctx,ins) do {		\
	RV32I_DECODE_I;				\
	rv32i_sltiu((ctx),_rd,_rs1,_imm);	\
    } while(0)

static inline void rv32i_xori(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,rdr(ctx,rs1) ^ imm);
}

#define RV32I_XORI(ctx,ins) do {		\
	RV32I_DECODE_I;				\
	rv32i_xori((ctx),_rd,_rs1,_imm);	\
    } while(0)

static inline void rv32i_slli(vasm_rt_t* ctx, int rd, int rs1, int shamt)
{
    wrr(ctx,rd,rdr(ctx,rs1) << shamt);
}

// fixme illegal if shamt[5] != 0
#define RV32I_SLLI(ctx,ins) do {			\
	RV32I_DECODE_I;					\
	rv32i_slli((ctx),_rd,_rs1,_imm & 0x1f);		\
    } while(0)

static inline void rv32i_srli(vasm_rt_t* ctx, int rd, int rs1, int shamt)
{
    wrr(ctx,rd, (ureg_t)rdr(ctx,rs1) >> shamt);
}

// fixme illegal if shamt[5] != 0
#define RV32I_SRLI(ctx,ins) do {			\
	RV32I_DECODE_I;					\
	rv32i_srli((ctx),_rd,_rs1,_imm & 0x1f);		\
    } while(0)

static inline void rv32i_srai(vasm_rt_t* ctx, int rd, int rs1, int shamt)
{
    wrr(ctx,rd,rdr(ctx,rs1) >> shamt);
}

#define RV32I_SRAI(ctx,ins) do {			\
	RV32I_DECODE_I;					\
	rv32i_srai((ctx),_rd,_rs1,_imm & 0x1f);		\
    } while(0)

static inline void rv32i_ori(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,rdr(ctx,rs1) | imm);
}

#define RV32I_ORI(ctx,ins) do {			\
	RV32I_DECODE_I;				\
	rv32i_ori((ctx),_rd,_rs1,_imm);		\
    } while(0)

static inline void rv32i_andi(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    wrr(ctx,rd,rdr(ctx,rs1) & imm);
}

#define RV32I_ANDI(ctx,ins) do {		\
	RV32I_DECODE_I;				\
	rv32i_andi((ctx),_rd,_rs1,_imm);	\
    } while(0)

static inline void rv32i_lb(vasm_rt_t* ctx, int rd, void* addr)
{
    wrr(ctx,rd,*((int8_t*)addr));
}

#define RV32I_LB(ctx,ins) do {						\
	RV32I_DECODE_I;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_lb((ctx),_rd,_addr);					\
    } while(0)

static inline void rv32i_lh(vasm_rt_t* ctx, int rd, void* addr)
{
    wrr(ctx,rd,*((int16_t*)addr));
}

#define RV32I_LH(ctx,ins) do {						\
	RV32I_DECODE_I;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_lh((ctx),_rd,_addr);					\
    } while(0)

static inline void rv32i_lw(vasm_rt_t* ctx, int rd, void* addr)
{
    wrr(ctx,rd,*((int32_t*)addr));
}

#define RV32I_LW(ctx,ins) do {						\
	RV32I_DECODE_I;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_lw((ctx),_rd,_addr);					\
    } while(0)

static inline void rv32i_lbu(vasm_rt_t* ctx, int rd, void* addr)
{
    wrr(ctx,rd,*((uint8_t*)addr));
}

#define RV32I_LBU(ctx,ins) do {						\
	RV32I_DECODE_I;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_lbu((ctx),_rd,_addr);					\
    } while(0)

static inline void rv32i_lhu(vasm_rt_t* ctx, int rd, void* addr)
{
    wrr(ctx,rd,*((uint16_t*)addr));
}

#define RV32I_LHU(ctx,ins) do {						\
	RV32I_DECODE_I;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_lhu((ctx),_rd,_addr);					\
    } while(0)

static inline void rv32i_sb(vasm_rt_t* ctx, int rs2, void* addr)
{
    *((uint8_t*)addr) = rdr(ctx,rs2);
}

#define RV32I_SB(ctx,ins) do {						\
	RV32I_DECODE_S;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_sb((ctx),_rs2,_addr);					\
    } while(0)

static inline void rv32i_sh(vasm_rt_t* ctx, int rs2, void* addr)
{
    *((uint16_t*)addr) = rdr(ctx,rs2);
}

#define RV32I_SH(ctx,ins) do {						\
	RV32I_DECODE_S;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_sh((ctx),_rs2,_addr);					\
    } while(0)

static inline void rv32i_sw(vasm_rt_t* ctx, int rs2, void* addr)
{
    *((uint32_t*)addr) = rdr(ctx,rs2);
}

#define RV32I_SW(ctx,ins) do {						\
	RV32I_DECODE_S;							\
	void* _addr = (void*) ((ctx)->mem + (rdr((ctx),_rs1)+_imm));	\
	rv32i_sw((ctx),_rs2,_addr);					\
    } while(0)

static inline int rv32i_beq(vasm_rt_t* ctx, int rs1, int rs2)
{
    return (rdr(ctx,rs1) == rdr(ctx,rs2));
}

static inline int rv32i_bne(vasm_rt_t* ctx, int rs1, int rs2)
{
    return (rdr(ctx,rs1) != rdr(ctx,rs2));
}

static inline int rv32i_blt(vasm_rt_t* ctx, int rs1, int rs2)
{
    return (rdr(ctx,rs1) < rdr(ctx,rs2));
}

static inline int rv32i_bge(vasm_rt_t* ctx, int rs1, int rs2)
{
    return (rdr(ctx,rs1) >= rdr(ctx,rs2));
}

static inline int rv32i_bltu(vasm_rt_t* ctx, int rs1, int rs2)
{
    return ((ureg_t)rdr(ctx,rs1) < (ureg_t)rdr(ctx,rs2));
}

static inline int rv32i_bgeu(vasm_rt_t* ctx, int rs1, int rs2)
{
    return ((ureg_t)rdr(ctx,rs1) >= (ureg_t)rdr(ctx,rs2));
}

static inline void rv32i_lui(vasm_rt_t* ctx, int rd, int imm)
{
    wrr(ctx,rd,(imm << 12));
}

#define RV32I_LUI(ctx,ins) do {						\
	RV32I_DECODE_U;							\
	rv32i_lui((ctx),_rd,_imm);					\
    } while(0)

// fixme: only in emu not ccode
static inline void rv32i_auipc(vasm_rt_t* ctx, int rd, int imm)
{
    wrr(ctx,rd, ctx->pc + (imm << 12));
}

#define RV32I_AUIPC(ctx,ins) do {					\
	RV32I_DECODE_U;							\
	rv32i_auipc((ctx),_rd,_imm);					\
    } while(0)

// fixme: only in emu not ccode, generate table jump
static inline int rv32i_jalr(vasm_rt_t* ctx, int rd, int rs1, int imm)
{
    reg_t pc1;
    wrr(ctx,rd,ctx->pc + 4);
    pc1 = (imm + rdr(ctx,rs1)) & ~1;
    return pc1;
}

// fixme: only in emu not ccode
static inline int rv32i_jal(vasm_rt_t* ctx, int rd, int imm)
{
    wrr(ctx,rd,ctx->pc + 4);
    return imm;
}

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

static inline void rv32i_ecall(vasm_rt_t* ctx)
{
    ctx->reg[10] = syscall(ctx->reg[10], ctx->reg[11],
			   ctx->reg[12], ctx->reg[13],
			   ctx->reg[14], ctx->reg[15],
			   ctx->reg[16], ctx->reg[17]);
}

#define RV32I_ECALL(ctx,ins) do {		\
	rv32i_ecall((ctx));			\
    } while(0)

#endif
