#include <ctype.h>
#include "vasm.h"

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

int asm_reg(token_t* tokens, int i, int* reg)
{
    if (tokens[i].c == TOKEN_REG) 
	*reg = tokens[i].value;
    else
	return -1;
    return i+1;
}

int asm_imm12(token_t* tokens, int i, int* imm)
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

    if (ctx->debug) {
	fprintf(stderr, "TOKENS: ");
	for (i = 0; i < num_tokens; i++)
	    fprintf(stderr, "[%s]", tokens[i].name);
	fprintf(stderr, "\n");
    }

    i = 0;
    if ((tokens[i].c == TOKEN_SYMBOL) && (tokens[i+1].c == ':')) {
	symbol_t* sym;
	sym = symbol_table_lookup(&ctx->symtab, tokens[i].name);
	if (sym == NULL)
	    sym = symbol_table_add(&ctx->symtab, tokens[0].name,
				   ctx->rt.mem_addr);
	else if (sym->flags & SYMBOL_FLAG_FORWARD) {
	    symbol_link_t* link = sym->forward;
	    sym->value = ctx->rt.mem_addr;
	    while(link) {
		int offset;
		symbol_link_t* nlink = link->next;
		instr_sb* instr = (instr_sb*) &ctx->rt.mem[link->addr];
		offset = sym->value - link->addr;
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
	if (eq_string(tokens[i].name, "add")) {
	    funct3 = FUNCT_ADD; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "sub")) {
	    funct3 = FUNCT_SUB; funct7 = 0x20; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "sll")) {
	    funct3 = FUNCT_SLL; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "slt")) {
	    funct3 = FUNCT_SLT; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "sltu")) { 
	    funct3 = FUNCT_SLTU; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "xor")) { 
	    funct3 = FUNCT_XOR; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "srl")) { 
	    funct3 = FUNCT_SRL; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "sra")) { 
	    funct3 = FUNCT_SRA; funct7 = 0x20; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "or")) { 
	    funct3 = FUNCT_OR; goto op_arith;
	}
	else if (eq_string(tokens[i].name, "and")) { 
	    funct3 = FUNCT_AND; goto op_arith;
	}
	if (eq_string(tokens[i].name, "addi")) {
	    funct3 = FUNCT_ADD; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "slli")) {
	    funct3 = FUNCT_SLL; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "slti")) {
	    funct3 = FUNCT_SLT; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "sltiu")) { 
	    funct3 = FUNCT_SLTU; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "xori")) { 
	    funct3 = FUNCT_XOR; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "srli")) { 
	    funct3 = FUNCT_SRL; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "srai")) { 
	    funct3 = FUNCT_SRA; imm11_0 = 0x20; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "ori")) { 
	    funct3 = FUNCT_OR; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "andi")) { 
	    funct3 = FUNCT_AND; goto op_imm;
	}
	else if (eq_string(tokens[i].name, "beq")) {
	    funct3 = FUNCT_BEQ; goto op_branch;
	}
	else if (eq_string(tokens[i].name, "bne")) {
	    funct3 = FUNCT_BNE; goto op_branch;
	}
	else if (eq_string(tokens[i].name, "blt")) {
	    funct3 = FUNCT_BLT; goto op_branch;
	}
	else if (eq_string(tokens[i].name, "bltu")) {
	    funct3 = FUNCT_BLTU; goto op_branch;
	}
	else if (eq_string(tokens[i].name, "bge")) {
	    funct3 = FUNCT_BGE; goto op_branch;
	}
	else if (eq_string(tokens[i].name, "bgeu")) {
	    funct3 = FUNCT_BGEU; goto op_branch;
	}
	else {
	    fprintf(stderr, "%s:%d unknown opcode %s\n", 
		    ctx->filename, ctx->lineno, tokens[i].name);
	    return -1;
	}

    op_arith: {
	    int rs1, rs2, rd;
	    instr_r* instr = (instr_r*) &ctx->rt.mem[ctx->rt.mem_addr];

	    i++;
	    if ((i = asm_reg(tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(tokens,i,&rs2)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_ARITH;
	    instr->rd = rd;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    instr->funct7 = funct7;

	    ctx->rt.mem_addr += 4;
	    return 0;
	}

    op_imm: {
	    int rs1, rd;
	    instr_i* instr = (instr_i*) &ctx->rt.mem[ctx->rt.mem_addr];
	    int imm;

	    i++;
	    if ((i = asm_reg(tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_imm12(tokens,i,&imm)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_IMM;
	    instr->rd = rd;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->imm11_0 = imm | imm11_0;

	    ctx->rt.mem_addr += 4;
	    return 0;
	}
    op_branch: {
	    int rs1, rs2;
	    instr_sb* instr = (instr_sb*) &ctx->rt.mem[ctx->rt.mem_addr];
	    int imm;

	    i++;
	    if ((i = asm_reg(tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reg(tokens,i,&rs2)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_imm12(tokens,i,&imm)) < 0)
		goto syntax_error;

	    instr->opcode = OPCODE_BRANCH;
	    instr->imm11 =  (imm>>11) & 0x1;
	    instr->imm4_1 = (imm>>1) & 0xf;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    instr->imm10_5 = (imm>>5) & 0x3f;
	    instr->imm12 = (imm>>12) & 0x1;

	    ctx->rt.mem_addr += 4;
	    return 0;
	}
    }

syntax_error:
    fprintf(stderr, "%s:%d: syntax error: %s\n", 
	    ctx->filename, ctx->lineno, ctx->linebuf);
    return -1;
}
