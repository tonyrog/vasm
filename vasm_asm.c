#include <ctype.h>
#include "vasm.h"

// static symbols

symbol_t symreg_xi[] =
{
    [0] = { .next = &symreg_xi[1], .flags = SYMBOL_FLAG_REG, .index = 0, .name = "x0"  },
    [1] = { .next = &symreg_xi[2], .flags = SYMBOL_FLAG_REG, .index = 1, .name = "x1"  },
    [2] = { .next = &symreg_xi[3], .flags = SYMBOL_FLAG_REG, .index = 2, .name = "x2"  },
    [3] = { .next = &symreg_xi[4], .flags = SYMBOL_FLAG_REG, .index = 3, .name = "x3"  },
    [4] = { .next = &symreg_xi[5], .flags = SYMBOL_FLAG_REG, .index = 4, .name = "x4"  },
    [5] = { .next = &symreg_xi[6], .flags = SYMBOL_FLAG_REG, .index = 5, .name = "x5"  },
    [6] = { .next = &symreg_xi[7], .flags = SYMBOL_FLAG_REG, .index = 6, .name = "x6"  },
    [7] = { .next = &symreg_xi[8], .flags = SYMBOL_FLAG_REG, .index = 7, .name = "x7"  },
    [8] = { .next = &symreg_xi[9], .flags = SYMBOL_FLAG_REG, .index = 8, .name = "x8"  },
    [9] = { .next = &symreg_xi[10], .flags = SYMBOL_FLAG_REG, .index = 9, .name = "x9"  },
    [10] = { .next = &symreg_xi[11], .flags = SYMBOL_FLAG_REG, .index = 10, .name = "x10"  },
    [11] = { .next = &symreg_xi[12], .flags = SYMBOL_FLAG_REG, .index = 11, .name = "x11"  },
    [12] = { .next = &symreg_xi[13], .flags = SYMBOL_FLAG_REG, .index = 12, .name = "x12"  },
    [13] = { .next = &symreg_xi[14], .flags = SYMBOL_FLAG_REG, .index = 13, .name = "x13"  },
    [14] = { .next = &symreg_xi[15], .flags = SYMBOL_FLAG_REG, .index = 14, .name = "x14"  },
    [15] = { .next = &symreg_xi[16], .flags = SYMBOL_FLAG_REG, .index = 15, .name = "x15"  },
    [16] = { .next = &symreg_xi[17], .flags = SYMBOL_FLAG_REG, .index = 16, .name = "x16"  },
    [17] = { .next = &symreg_xi[18], .flags = SYMBOL_FLAG_REG, .index = 17, .name = "x17"  },
    [18] = { .next = &symreg_xi[19], .flags = SYMBOL_FLAG_REG, .index = 18, .name = "x18"  },
    [19] = { .next = &symreg_xi[20], .flags = SYMBOL_FLAG_REG, .index = 19, .name = "x19"  },
    [20] = { .next = &symreg_xi[21], .flags = SYMBOL_FLAG_REG, .index = 20, .name = "x20"  },
    [21] = { .next = &symreg_xi[22], .flags = SYMBOL_FLAG_REG, .index = 21, .name = "x21"  },
    [22] = { .next = &symreg_xi[23], .flags = SYMBOL_FLAG_REG, .index = 22, .name = "x22"  },
    [23] = { .next = &symreg_xi[24], .flags = SYMBOL_FLAG_REG, .index = 23, .name = "x23"  },
    [24] = { .next = &symreg_xi[25], .flags = SYMBOL_FLAG_REG, .index = 24, .name = "x24"  },
    [25] = { .next = &symreg_xi[26], .flags = SYMBOL_FLAG_REG, .index = 25, .name = "x25"  },
    [26] = { .next = &symreg_xi[27], .flags = SYMBOL_FLAG_REG, .index = 26, .name = "x26"  },
    [27] = { .next = &symreg_xi[28], .flags = SYMBOL_FLAG_REG, .index = 27, .name = "x27"  },
    [28] = { .next = &symreg_xi[29], .flags = SYMBOL_FLAG_REG, .index = 28, .name = "x28"  },
    [29] = { .next = &symreg_xi[30], .flags = SYMBOL_FLAG_REG, .index = 29, .name = "x29"  },
    [30] = { .next = &symreg_xi[31], .flags = SYMBOL_FLAG_REG, .index = 30, .name = "x30"  },
    [31] = { .next = 0, .flags = SYMBOL_FLAG_REG, .index = 31, .name = "x31"  }
};

char* register_xi_name(int r)
{
    return symreg_xi[r].name;
}

symbol_t symreg_abi[] = 
{
    [0] = { .next = &symreg_abi[1], .flags = SYMBOL_FLAG_REG, .index = 0, .name = "zero"  },
    [1] = { .next = &symreg_abi[2], .flags = SYMBOL_FLAG_REG, .index = 1, .name = "ra"  },
    [2] = { .next = &symreg_abi[3], .flags = SYMBOL_FLAG_REG, .index = 2, .name = "fp"  },
    [3] = { .next = &symreg_abi[4], .flags = SYMBOL_FLAG_REG, .index = 3, .name = "s1"  },
    [4] = { .next = &symreg_abi[5], .flags = SYMBOL_FLAG_REG, .index = 4, .name = "s2"  },
    [5] = { .next = &symreg_abi[6], .flags = SYMBOL_FLAG_REG, .index = 5, .name = "s3"  },
    [6] = { .next = &symreg_abi[7], .flags = SYMBOL_FLAG_REG, .index = 6, .name = "s4"  },
    [7] = { .next = &symreg_abi[8], .flags = SYMBOL_FLAG_REG, .index = 7, .name = "s5"  },
    [8] = { .next = &symreg_abi[9], .flags = SYMBOL_FLAG_REG, .index = 8, .name = "s6"  },
    [9] = { .next = &symreg_abi[10], .flags = SYMBOL_FLAG_REG, .index = 9, .name = "s7"  },
    [10] = { .next = &symreg_abi[11], .flags = SYMBOL_FLAG_REG, .index = 10, .name = "s8"  },
    [11] = { .next = &symreg_abi[12], .flags = SYMBOL_FLAG_REG, .index = 11, .name = "s9"  },
    [12] = { .next = &symreg_abi[13], .flags = SYMBOL_FLAG_REG, .index = 12, .name = "s10"  },
    [13] = { .next = &symreg_abi[14], .flags = SYMBOL_FLAG_REG, .index = 13, .name = "s11"  },
    [14] = { .next = &symreg_abi[15], .flags = SYMBOL_FLAG_REG, .index = 14, .name = "sp"  },
    [15] = { .next = &symreg_abi[16], .flags = SYMBOL_FLAG_REG, .index = 15, .name = "tp"  },
    [16] = { .next = &symreg_abi[17], .flags = SYMBOL_FLAG_REG, .index = 16, .name = "v0"  },
    [17] = { .next = &symreg_abi[18], .flags = SYMBOL_FLAG_REG, .index = 17, .name = "v1"  },
    [18] = { .next = &symreg_abi[19], .flags = SYMBOL_FLAG_REG, .index = 18, .name = "a0"  },
    [19] = { .next = &symreg_abi[20], .flags = SYMBOL_FLAG_REG, .index = 19, .name = "a1"  },
    [20] = { .next = &symreg_abi[21], .flags = SYMBOL_FLAG_REG, .index = 20, .name = "a2"  },
    [21] = { .next = &symreg_abi[22], .flags = SYMBOL_FLAG_REG, .index = 21, .name = "a3"  },
    [22] = { .next = &symreg_abi[23], .flags = SYMBOL_FLAG_REG, .index = 22, .name = "a4"  },
    [23] = { .next = &symreg_abi[24], .flags = SYMBOL_FLAG_REG, .index = 23, .name = "a5"  },
    [24] = { .next = &symreg_abi[25], .flags = SYMBOL_FLAG_REG, .index = 24, .name = "a6"  },
    [25] = { .next = &symreg_abi[26], .flags = SYMBOL_FLAG_REG, .index = 25, .name = "a7"  },
    [26] = { .next = &symreg_abi[27], .flags = SYMBOL_FLAG_REG, .index = 26, .name = "t0"  },
    [27] = { .next = &symreg_abi[28], .flags = SYMBOL_FLAG_REG, .index = 27, .name = "t1"  },
    [28] = { .next = &symreg_abi[29], .flags = SYMBOL_FLAG_REG, .index = 28, .name = "t2"  },
    [29] = { .next = &symreg_abi[30], .flags = SYMBOL_FLAG_REG, .index = 29, .name = "t3"  },
    [30] = { .next = &symreg_abi[31], .flags = SYMBOL_FLAG_REG, .index = 30, .name = "gp"  },
    [31] = { .next = 0, .flags = SYMBOL_FLAG_REG, .index = 2,  .name = "s0"  }
};

char* register_abi_name(int r)
{
    return symreg_abi[r].name;
}

symbol_t sym_instr[] =
{
    [0] = { .next = &sym_instr[1], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_ADD_SI, .name = "add" },
    [1] = { .next = &sym_instr[2], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SUB_SI, .name = "sub" },
    [2] = { .next = &sym_instr[3], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLL_SI, .name = "sll" },
    [3] = { .next = &sym_instr[4], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLT_SI, .name = "slt" },
    [4] = { .next = &sym_instr[5], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLTU_SI, .name = "sltu" },
    [5] = { .next = &sym_instr[6], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_XOR_SI, .name = "xor" },
    [6] = { .next = &sym_instr[7], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SRL_SI, .name = "srl" },
    [7] = { .next = &sym_instr[8], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SRA_SI, .name = "sra" },
    [8] = { .next = &sym_instr[9], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_OR_SI, .name = "or" },
    [9] = { .next = &sym_instr[10], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_AND_SI, .name = "and" },
    [10] = { .next = &sym_instr[11], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_ADDI_SI, .name = "addi" },
    [11] = { .next = &sym_instr[12], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLLI_SI, .name = "slli" },
    [12] = { .next = &sym_instr[13], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLTI_SI, .name = "slti" },
    [13] = { .next = &sym_instr[14], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SLTIU_SI, .name = "sltiu" },
    [14] = { .next = &sym_instr[15], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_XORI_SI, .name = "xori" },
    [15] = { .next = &sym_instr[16], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SRLI_SI, .name = "srli" },
    [16] = { .next = &sym_instr[17], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_SRAI_SI, .name = "srai" },
    [17] = { .next = &sym_instr[18], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_ORI_SI, .name = "ori" },
    [18] = { .next = &sym_instr[19], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_ANDI_SI, .name = "andi" },
    [19] = { .next = &sym_instr[20], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BEQ_SI, .name = "beq" },
    [20] = { .next = &sym_instr[21], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BNE_SI, .name = "bne" },
    [21] = { .next = &sym_instr[22], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BLT_SI, .name = "blt" },
    [22] = { .next = &sym_instr[23], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BLTU_SI, .name = "bltu" },
    [23] = { .next = &sym_instr[24], .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BGE_SI, .name = "bge" },
    [24] = { .next = 0,              .flags = SYMBOL_FLAG_INSTR, .index = INSTR_BGEU_SI, .name = "bgeu" },
};

#define NINSTR (sizeof(sym_instr)/sizeof(symbol_t))

void asm_init(symbol_table_t* symtab) 
{
    // link in static symbols above into symtab
    symbol_table_install(symtab, &symreg_abi[0]);
    symbol_table_install(symtab, &symreg_xi[0]);
    symbol_table_install(symtab, &sym_instr[0]);
}

int eq_string(char* ptr, char* name)
{
    return (strcmp(ptr, name) == 0);
}

// parse integer [-|+][0x|0b]dddddd
int to_int(char* ptr)
{
    int val = 0;
    int sign = 0;
    int base = 10;

    if (*ptr == '-') {
	sign = -1;
	ptr++;
    }
    else if (*ptr == '+')
	ptr++;

    if ((ptr[0] == '0') && (ptr[1] == 'x')) {
	base = 16;
	ptr += 2;
    }
    else if ((ptr[0] == '0') && (ptr[1] == 'b')) {
	base = 2;
	ptr += 2;
    }
    while(*ptr) {
	if (isdigit(*ptr))
	    val = val*base + (*ptr - '0');
	else if ((*ptr >= 'A') && (*ptr <= 'F') && (base == 16))
	    val = val*16 + ((*ptr-'A')+10);
	else if ((*ptr >= 'a') && (*ptr <= 'f') && (base == 16))
	    val = val*16 + ((*ptr-'a')+10);
	ptr++;
    }
    if (sign < 0)
	return -val;
    return val;
}

signed_t lookup_jump_target(symbol_table_t* symtab, char* label, unsigned addr)
{
    symbol_t* sym;
    if ((sym = symbol_table_lookup(symtab, label)) == NULL) { // maybe forward
	sym = symbol_table_add(symtab, label, 0);
	symbol_link_add(sym, addr);
    }
    else {
	if (sym->flags & SYMBOL_FLAG_FORWARD)
	    symbol_link_add(sym, addr);
	else if (sym->flags & SYMBOL_FLAG_LABEL)
	    return sym->value - addr;
    }
    return 0;
}

int asm_reg(vasm_ctx_t* ctx, token_t* tokens, int i, int* reg)
{
    if (tokens[i].c == TOKEN_SYMBOL) {
	symbol_t* symr = symbol_table_lookup(&ctx->symtab, tokens[i].name);
	if ((symr != NULL) && ((symr->flags & SYMBOL_FLAG_REG) != 0)) {
	    *reg = symr->index;
	    return i+1;
	}
    }
    return -1;
}

int asm_imm12(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

//
// instruction format
// [<label> ':'] [<instruction> [ <operand> (',' <operand> )* ] ]
//
int assemble(vasm_ctx_t* ctx, token_t* tokens, size_t num_tokens)
{
    char* ptr = ctx->linebuf;
    char* wp;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0x00;
    signed   imm11_0 = 0;
    int i;

#ifdef HARD_DEBUG
    if (ctx->debug) {
	fprintf(stderr, "TOKENS: ");
	for (i = 0; i < num_tokens; i++)
	    fprintf(stderr, "[%s]", tokens[i].name);
	fprintf(stderr, "\n");
    }
#endif

    i = 0;
    if ((tokens[i].c == TOKEN_SYMBOL) && (tokens[i+1].c == ':')) {
	symbol_t* sym;
	sym = symbol_table_lookup(&ctx->symtab, tokens[i].name);
	if (sym == NULL)
	    sym = symbol_table_add(&ctx->symtab, tokens[0].name,ctx->rt.waddr);
	else if (sym->flags & SYMBOL_FLAG_FORWARD) {
	    symbol_link_t* link = sym->forward;
	    sym->value = ctx->rt.waddr;
	    while(link) {
		int offset;
		symbol_link_t* nlink = link->next;
		instr_sb* instr = (instr_sb*) &ctx->rt.mem[link->addr];
		offset = sym->value - link->addr;
		// fixme: check offset range!
		instr->imm4_1  = (offset>>1) & 0xf;
		instr->imm10_5 = (offset>>5) & 0x3f;
		instr->imm11   = (offset>>11) & 0x1;
		instr->imm12   = (offset>>12) & 0x1;
		symbol_link_free(link);
		link = nlink;
	    }
	    sym->flags &= ~SYMBOL_FLAG_FORWARD;
	}
	sym->flags |= SYMBOL_FLAG_LABEL;
	i += 2;
    }

    if (i == num_tokens)
	return 0;
    
    if (tokens[i].c == TOKEN_SYMBOL) {
	symbol_t* isym = symbol_table_lookup(&ctx->symtab, tokens[i].name);
	if ((isym == NULL) || ((isym->flags & SYMBOL_FLAG_INSTR) == 0)) {
	    fprintf(stderr, "%s:%d unknown opcode %s\n", 
		    ctx->filename, ctx->lineno, tokens[i].name);
	    return -1;
	}
	switch(isym->index) {
	case INSTR_ADD_SI: funct3 = FUNCT_ADD; goto op_arith;
	case INSTR_SUB_SI: funct3 = FUNCT_SUB; funct7 = 0x20; goto op_arith;
	case 2: funct3 = FUNCT_SLL; goto op_arith;
	case 3: funct3 = FUNCT_SLT; goto op_arith;
	case 4: funct3 = FUNCT_SLTU; goto op_arith;
	case 5: funct3 = FUNCT_XOR; goto op_arith;
	case 6: funct3 = FUNCT_SRL; goto op_arith;
	case 7: funct3 = FUNCT_SRA; funct7 = 0x20; goto op_arith;
	case 8: funct3 = FUNCT_OR; goto op_arith;
	case 9: funct3 = FUNCT_AND; goto op_arith;
	case 10: funct3 = FUNCT_ADDI; goto op_imm;
	case 11: funct3 = FUNCT_SLLI; goto op_imm;
	case 12: funct3 = FUNCT_SLTI; goto op_imm;
	case 13: funct3 = FUNCT_SLTIU; goto op_imm;
	case 14: funct3 = FUNCT_XORI; goto op_imm;
	case 15: funct3 = FUNCT_SRLI; goto op_imm;
	case 16: funct3 = FUNCT_SRAI; imm11_0 = 0x20; goto op_imm;
	case 17: funct3 = FUNCT_ORI; goto op_imm;
	case 18: funct3 = FUNCT_ANDI; goto op_imm;
	case 19: funct3 = FUNCT_BEQ; goto op_branch;
	case 20: funct3 = FUNCT_BNE; goto op_branch;
	case 21: funct3 = FUNCT_BLT; goto op_branch;
	case 22: funct3 = FUNCT_BLTU; goto op_branch;
	case 23: funct3 = FUNCT_BGE; goto op_branch;
	case 24: funct3 = FUNCT_BGEU; goto op_branch;
	default:
	    fprintf(stderr, "%s:%d unknown opcode %s\n", 
		    ctx->filename, ctx->lineno, tokens[i].name);
	    return -1;
	}

    op_arith: {
	    int rs1, rs2, rd;
	    instr_r* instr = (instr_r*) &ctx->rt.mem[ctx->rt.waddr];

	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rs2)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_ARITH;
	    instr->rd = rd;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    instr->funct7 = funct7;

	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_imm: {
	    int rs1, rd;
	    instr_i* instr = (instr_i*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_imm12(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_IMM;
	    instr->rd = rd;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->imm11_0 = imm | imm11_0;

	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_branch: {
	    int rs1, rs2;
	    instr_sb* instr = (instr_sb*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(ctx,tokens,i,&rs2)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_imm12(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_BRANCH;
	    instr->imm11 =  (imm>>11) & 0x1;
	    instr->imm4_1 = (imm>>1) & 0xf;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    instr->imm10_5 = (imm>>5) & 0x3f;
	    instr->imm12 = (imm>>12) & 0x1;

	    ctx->rt.waddr += 4;
	    return 0;
	}
    }

syntax_error:
    fprintf(stderr, "%s:%d: syntax error: %s\n", 
	    ctx->filename, ctx->lineno, ctx->linebuf);
    return -1;
}
