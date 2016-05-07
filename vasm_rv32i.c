#include <ctype.h>
#include "vasm.h"

#include "vasm_rv32i.h"

#define SYMREG_XI(i, nm) \
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG, .index = (i), .name = (nm)  }

symbol_t sym_reg_xi[] =
{
    SYMREG_XI(0, "x0"),
    SYMREG_XI(1, "x1"),
    SYMREG_XI(2, "x2"),
    SYMREG_XI(3, "x3"),
    SYMREG_XI(4, "x4"),
    SYMREG_XI(5, "x5"),
    SYMREG_XI(6, "x6"),
    SYMREG_XI(7, "x7"),
    SYMREG_XI(8, "x8"),
    SYMREG_XI(9, "x9"),
    SYMREG_XI(10, "x10"),
    SYMREG_XI(11, "x11"),
    SYMREG_XI(12, "x12"),
    SYMREG_XI(13, "x13"),
    SYMREG_XI(14, "x14"),
    SYMREG_XI(15, "x15"),
    SYMREG_XI(16, "x16"),
    SYMREG_XI(17, "x17"),
    SYMREG_XI(18, "x18"),
    SYMREG_XI(19, "x19"),
    SYMREG_XI(20, "x20"),
    SYMREG_XI(21, "x21"),
    SYMREG_XI(22, "x22"),
    SYMREG_XI(23, "x23"),
    SYMREG_XI(24, "x24"),
    SYMREG_XI(25, "x25"),
    SYMREG_XI(26, "x26"),
    SYMREG_XI(27, "x27"),
    SYMREG_XI(28, "x28"),
    SYMREG_XI(29, "x29"),
    SYMREG_XI(30, "x30"),
    SYMREG_XI(31, "x31"),
};

char* register_xi_name(int r)
{
    return sym_reg_xi[r].name;
}

#define SYMREG_ABI(i, nm) \
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG, .index = (i), .name = (nm)  }

#define SYMREG_ABI_IX(i,ix,nm)						\
    [(i)] = { .next = 0, .flags = SYMBOL_FLAG_REG, .index = (ix), .name = (nm)  }

symbol_t sym_reg_abi[] = 
{
    SYMREG_ABI(0, "zero"),
    SYMREG_ABI(1, "ra"),   
    SYMREG_ABI(2, "fp"),
    SYMREG_ABI(3, "s1"),
    SYMREG_ABI(4, "s2"),
    SYMREG_ABI(5, "s3"),
    SYMREG_ABI(6, "s4"),
    SYMREG_ABI(7, "s5"),
    SYMREG_ABI(8, "s6"),
    SYMREG_ABI(9, "s7"),
    SYMREG_ABI(10, "s8"),
    SYMREG_ABI(11, "s9"),
    SYMREG_ABI(12, "s10"),
    SYMREG_ABI(13, "s11"),
    SYMREG_ABI(14, "sp"),
    SYMREG_ABI(15, "tp"),
    SYMREG_ABI(16, "v0"),
    SYMREG_ABI(17, "v1"),
    SYMREG_ABI(18, "a0"),
    SYMREG_ABI(19, "a1"),
    SYMREG_ABI(20, "a2"),
    SYMREG_ABI(21, "a3"),
    SYMREG_ABI(22, "a4"),
    SYMREG_ABI(23, "a5"),
    SYMREG_ABI(24, "a6"),
    SYMREG_ABI(25, "a7"),
    SYMREG_ABI(26, "t0"),
    SYMREG_ABI(27, "t1"),
    SYMREG_ABI(28, "t2"),
    SYMREG_ABI(29, "t3"),
    SYMREG_ABI(30, "t4"),
    SYMREG_ABI(31, "gp"),
    SYMREG_ABI_IX(32, 2, "s0"),
};

char* register_abi_name(int r)
{
    return sym_reg_abi[r].name;
}

symbol_t sym_instr_rv32i[] =
{
    SYM_INSTR_X(add, OPCODE_ARITH, FUNCT_ADD, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(sub, OPCODE_ARITH, ((0x20)<<3)|FUNCT_SUB, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),

    SYM_INSTR_X(sll, OPCODE_ARITH, FUNCT_SLL, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(srl, OPCODE_ARITH, FUNCT_SLL, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(sra, OPCODE_ARITH, ((0x20)<<3)|FUNCT_SRA, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(and, OPCODE_ARITH, FUNCT_AND, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(or, OPCODE_ARITH, FUNCT_OR, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(xor, OPCODE_ARITH, FUNCT_OR, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(slt, OPCODE_ARITH, FUNCT_SLT, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),
    SYM_INSTR_X(sltu, OPCODE_ARITH, FUNCT_SLTU, FORMAT_R, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_REG_RS2)),

    SYM_INSTR_X(addi, OPCODE_IMM, FUNCT_ADDI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(slli, OPCODE_IMM, FUNCT_SLLI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_5)),
    SYM_INSTR_X(srli, OPCODE_IMM, FUNCT_SRLI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_5)),
    SYM_INSTR_X(srai, OPCODE_IMM, (0x20<<3)|FUNCT_SRAI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_5)),

    SYM_INSTR_X(andi, OPCODE_IMM, FUNCT_ANDI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(ori, OPCODE_IMM, FUNCT_ORI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(xori, OPCODE_IMM, FUNCT_XORI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(slti, OPCODE_IMM, FUNCT_SLTI, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(sltiu, OPCODE_IMM, FUNCT_SLTIU, FORMAT_I, 0,
		ASM_SEQ3(ASM_REG_RD, ASM_REG_RS1, ASM_IMM_12)),
    SYM_INSTR_X(lui, OPCODE_LUI, 0, FORMAT_U, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_20)),
    SYM_INSTR_X(auipc, OPCODE_AUIPC, 0, FORMAT_U, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_20)),
    SYM_INSTR_X(lb, OPCODE_LOAD, FUNCT_LB, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(lbu, OPCODE_LOAD, FUNCT_LBU, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(lh, OPCODE_LOAD, FUNCT_LH, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(lhu, OPCODE_LOAD, FUNCT_LHU, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(lw, OPCODE_LOAD, FUNCT_LW, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RS2, ASM_IMM_12_RS1)),

    SYM_INSTR_X(sb, OPCODE_STORE, FUNCT_SB, FORMAT_S, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(sh, OPCODE_STORE, FUNCT_SH, FORMAT_S, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),
    SYM_INSTR_X(sw, OPCODE_STORE, FUNCT_SW, FORMAT_S, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_IMM_12_RS1)),

    SYM_INSTR_X(fence, OPCODE_FENCE, FUNCT_FENCE, FORMAT_I, 0,
		ASM_SEQ2(ASM_IMM_IORW, ASM_IMM_IORW)),
    SYM_INSTR_NAME_X(fence_i, "fence.i", OPCODE_FENCE, FUNCT_FENCE_I, FORMAT_I, 0,
		     ASM_SEQ0()),

    SYM_INSTR_X(beq, OPCODE_BRANCH, FUNCT_BEQ, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)), 

    SYM_INSTR_X(bne, OPCODE_BRANCH, FUNCT_BNE, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)),
    SYM_INSTR_X(blt, OPCODE_BRANCH, FUNCT_BLT, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)),
    SYM_INSTR_X(bltu, OPCODE_BRANCH, FUNCT_BLTU, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)), 
    SYM_INSTR_X(bge, OPCODE_BRANCH, FUNCT_BGE, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)), 
    SYM_INSTR_X(bgeu, OPCODE_BRANCH, FUNCT_BGEU, FORMAT_SB, 0,
		ASM_SEQ3(ASM_REG_RS1, ASM_REG_RS2, ASM_REL_12)), 
    SYM_INSTR_X(jal, OPCODE_JAL, 0, FORMAT_UJ, 0,
		ASM_SEQ2(ASM_REG_RD, ASM_REL_20)), 
    SYM_INSTR_X(jalr, OPCODE_JALR, 0, FORMAT_I, 0,
		ASM_SEQ2(ASM_REG_RS2, ASM_IMM_12_RS1)),

    SYM_INSTR_X(scall, OPCODE_SYS, 0, FORMAT_I, 0x000,
		 ASM_SEQ0()),
    SYM_INSTR_X(sbreak, OPCODE_SYS, 0, FORMAT_I, 0x001,
		ASM_SEQ0()),
    SYM_INSTR_X(rdcycle, OPCODE_SYS, 0, FORMAT_I, 0xc00,
		ASM_SEQ1(ASM_REG_RD)),
    SYM_INSTR_X(rdcycleh,OPCODE_SYS, 0, FORMAT_I, 0xc80,
		ASM_SEQ1(ASM_REG_RD)),
    SYM_INSTR_X(rdtime,OPCODE_SYS, 0, FORMAT_I, 0xc01,
		ASM_SEQ1(ASM_REG_RD)),
    SYM_INSTR_X(rdtimeh,OPCODE_SYS, 0, FORMAT_I, 0xc81,
		ASM_SEQ1(ASM_REG_RD)),
    SYM_INSTR_X(rdinstret,OPCODE_SYS, 0, FORMAT_I, 0xc02,
		ASM_SEQ1(ASM_REG_RD)),
    SYM_INSTR_X(rdinstreth,OPCODE_SYS, 0, FORMAT_I, 0xc82,
	       ASM_SEQ1(ASM_REG_RD)),

    // pseduo instructions

    // nop = addi x0,x0,0
    SYM_INSTR_X(nop, OPCODE_IMM, FUNCT_ADDI, FORMAT_I, 0,
		ASM_SEQ0()),

    // j = jal x0,<imm>
    SYM_INSTR_X(j, OPCODE_JAL, 0, FORMAT_UJ, 0,
		ASM_SEQ1(ASM_REL_20)),

    // li = addi <r>,x0,<imm>
    SYM_INSTR_X(li, OPCODE_IMM, FUNCT_ADDI, FORMAT_I, 0,
                ASM_SEQ2(ASM_REG_RD, ASM_IMM_12)),

    // inc = addi <r>,<r>,1
    SYM_INSTR_X(inc, OPCODE_IMM, FUNCT_ADDI, FORMAT_I, 0,
                ASM_SEQ3(ASM_REG_RD, ASM_COPY_RD_RS1, ASM_CONST_1)),
    // dec = addi <r>,<r>,-1
    SYM_INSTR_X(dec, OPCODE_IMM, FUNCT_ADDI, FORMAT_I, 0,
                ASM_SEQ3(ASM_REG_RD, ASM_COPY_RD_RS1, ASM_CONST_MINUS_1)),

};

int vasm_rv32i_asm_init(symbol_table_t* symtab)
{
    // link in static symbols above into symtab
    symbol_table_install_array(symtab, &sym_reg_abi[0], 
			       sizeof_array(sym_reg_abi));
    symbol_table_install_array(symtab, &sym_reg_xi[0],
			       sizeof_array(sym_reg_xi));
    symbol_table_install_array(symtab, &sym_instr_rv32i[0],
			       sizeof_array(sym_instr_rv32i));
    return 0;
}
