#include <ctype.h>
#include "vasm.h"

#define HARD_DEBUG

void asm_init(symbol_table_t* symtab) 
{
    vasm_rv32i_asm_init(symtab);
#if defined(RV32M)
    vasm_rv32m_asm_init(symtab);
#endif
#if defined(RV32C)
    vasm_rv32c_asm_init(symtab);
#endif
}

// parse integer [-|+][0x|0b]dddddd
int to_int(char* ptr)
{
    int val = 0;
    int sign = 0;
    int base = 10;

    // fprintf(stderr, "to_int: ptr=%s\n", ptr);
    
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
	sym->flags |= SYMBOL_FLAG_FORWARD;
	symbol_link_add(sym, addr);
	// fprintf(stderr, "forward label %s @ %d\n", label, addr);
    }
    else {
	if (sym->flags & SYMBOL_FLAG_FORWARD) {
	    // fprintf(stderr, "forward label link %s @ %d\n", label, addr);
	    symbol_link_add(sym, addr);
	}
	else if (sym->flags & SYMBOL_FLAG_LABEL) {
	    // fprintf(stderr, "label %s offset %d\n", label, sym->value - addr);
	    return sym->value - addr;
	}
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

int asm_iorw(vasm_ctx_t* ctx, token_t* tokens, int i, int* iorw)
{
    char* ptr;
    int v = 0;
    if (tokens[i].c != TOKEN_SYMBOL)
	return -1;
    ptr = tokens[i].name;
    while(*ptr) {
	switch(*ptr) {
	case 'i': v |= FENCE_I_BIT; break;
	case 'o': v |= FENCE_O_BIT; break;
	case 'r': v |= FENCE_R_BIT; break;
	case 'w': v |= FENCE_W_BIT; break;
	default: return -1;
	}
	ptr++;
    }
    *iorw = v;
    return i+1;
}

int asm_imm5(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm6(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm8(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm12(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm20(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_reladdr12(vasm_ctx_t* ctx, token_t* tokens, int i, int32_t* imm)
{
    if (tokens[i].c == TOKEN_SYMBOL) {
	*imm = lookup_jump_target(&ctx->symtab, tokens[i].name, ctx->rt.waddr);
	return i+1;
    }
    return asm_imm12(ctx, tokens, i, imm);
}

int asm_reladdr20(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
{
    if (tokens[i].c == TOKEN_SYMBOL) {
	*imm = lookup_jump_target(&ctx->symtab, tokens[i].name, ctx->rt.waddr);
	return i+1;
    }
    return asm_imm20(ctx, tokens, i, imm);
}


//
// instruction format
// [<label> ':'] [<instruction> [ <operand> (',' <operand> )* ] ]
//
int assemble(vasm_ctx_t* ctx, token_t* tokens, size_t num_tokens)
{
    int32_t  imm=0;
    int      rd=0, rs1=0, rs2=0;
    int      i, j;
    uint32_t seq;
    symbol_t* isym;

#define NEXT_ARG if (j) { if (tokens[i].c != ',') goto syntax_error; i++; } else { j++; }

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
	if (sym == NULL) {
	    if (ctx->debug) {
		fprintf(stderr, "define label %s @ %d\n", 
			tokens[0].name,ctx->rt.waddr);
	    }
	    sym = symbol_table_add(&ctx->symtab, tokens[0].name,ctx->rt.waddr);
	}
	else if (sym->flags & SYMBOL_FLAG_FORWARD) {
	    symbol_link_t* link = sym->forward;
	    sym->value = ctx->rt.waddr;
	    while(link) {
		symbol_link_t* nlink = link->next;
		uint32_t* instr = (uint32_t*) &ctx->rt.mem[link->addr];

		if (ctx->debug) {
		    fprintf(stderr, "resolve label %s @ %x = %d\n", 
			    tokens[0].name,link->addr,
			    sym->value - link->addr);
		}
		*instr = set_imm_sb(*instr, sym->value - link->addr);
		symbol_link_free(link);
		link = nlink;
	    }
	    sym->flags &= ~SYMBOL_FLAG_FORWARD;
	}
	sym->flags |= SYMBOL_FLAG_LABEL;
	i += 2;
    }

    // fprintf(stderr, "i=%d, num_tokens=%zu\n", i, num_tokens);
    if (i == num_tokens)
	return 0;

    if (tokens[i].c != TOKEN_SYMBOL)
	goto syntax_error;
    
    isym = symbol_table_lookup(&ctx->symtab, tokens[i].name);
    // fprintf(stderr, "isym = %p\n", isym);
    if ((isym == NULL) || ((isym->flags & SYMBOL_FLAG_INSTR) == 0)) {
	fprintf(stderr, "%s:%d unknown opcode %s\n", 
		ctx->filename, ctx->lineno, tokens[i].name);
	return -1;
    }
    i++;

    // parse arguments according to symbol sequence info
    seq = isym->sequence;
    j = 0;

    while(seq) {
	switch(seq & 0x1f) {
	case ASM_COPY_RD_RS1:  // copy RD to RS1
	    rs1 = rd;
	    break;
	case ASM_CONST_1:
	    imm = 1;
	    break;
	case ASM_CONST_MINUS_1:
	    imm = -1;
	    break;
	case ASM_REG_RD:
	    NEXT_ARG;
	    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		goto syntax_error;
	    break;
	case ASM_REG_RS1:
	    NEXT_ARG;
	    if ((i = asm_reg(ctx,tokens,i,&rs1)) < 0)
		goto syntax_error;
	    break;
	case ASM_REG_RS2:
	    NEXT_ARG;
	    if ((i = asm_reg(ctx,tokens,i,&rs2)) < 0)
		goto syntax_error;
	    break;
	case ASM_IMM_5:
	    NEXT_ARG;
	    if ((i = asm_imm5(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_IMM_6:
	    NEXT_ARG;
	    if ((i = asm_imm6(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_IMM_8:
	    NEXT_ARG;
	    if ((i = asm_imm8(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_IMM_12:
	    NEXT_ARG;
	    if ((i = asm_imm12(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_UIMM_20:  // load upper 20 bits
	    NEXT_ARG;
	    if ((i = asm_imm20(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    imm >>= 12;
	    break;
	case ASM_REL_12:
	    NEXT_ARG;
	    if ((i = asm_reladdr12(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_REL_20:
	    NEXT_ARG;
	    if ((i = asm_reladdr20(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    break;
	case ASM_IMM_12_RS1:  // optional imm12 (or 0) and rs1
	    NEXT_ARG;
	    if (tokens[i].c == '(')
		i++;
	    else {
		if ((i = asm_imm12(ctx,tokens,i,&imm)) < 0)
		    goto syntax_error;
		if (tokens[i].c != '(')
		    goto syntax_error;
		i++;
	    }
	    if ((i = asm_reg(ctx,tokens,i,&rs1)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ')')
		goto syntax_error;
	    break;
	case ASM_IMM_IORW: {
	    int tmp;
	    NEXT_ARG;
	    imm <<= 4;
	    if ((i = asm_iorw(ctx,tokens,i,&tmp)) < 0)
		goto syntax_error;
	    imm |= tmp;
	    break;
	}
	default:
	    fprintf(stderr, "%s:%d internal error, %s sequence %d\n",
		    ctx->filename, ctx->lineno, tokens[i].name, seq & 0xf);	
	    return -1;
	}
	seq >>= 5;
	j++;
    }

    switch(isym->format) {
    case FORMAT_R: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_r, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_r, rd, ins, rd);
	ins = bitfield_store(instr_r, funct3, ins, isym->funct & 0x7);
	ins = bitfield_store(instr_r, rs1, ins, rs1);
	ins = bitfield_store(instr_r, rs2, ins, rs2);
	ins = bitfield_store(instr_r, funct7, ins, isym->funct >> 3);
	*instr = ins;
	fprintf(stderr, "%08x: format-r ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_I: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_i, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_i, rd, ins, rd);
	ins = bitfield_store(instr_i, funct3, ins, isym->funct & 0x7);
	ins = bitfield_store(instr_i, rs1, ins, rs1);
	ins = bitfield_store(instr_i, imm11_0, ins, imm | isym->value);
	*instr = ins;
	fprintf(stderr, "%08x: format-i ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_S: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_s, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_s, rs1, ins, rs1);
	ins = bitfield_store(instr_s, rs2, ins, rs2);
	ins = bitfield_store(instr_s, funct3, ins, isym->funct & 0x7);
	ins = set_imm_s(ins, imm);
	*instr = ins;
	fprintf(stderr, "%08x: format-s ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_SB: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_sb, opcode, ins, isym->opcode);
	ins = set_imm_sb(ins, imm);
	ins = bitfield_store(instr_sb, funct3, ins, isym->funct & 0x7);
	ins = bitfield_store(instr_sb, rs1, ins, rs1);
	ins = bitfield_store(instr_sb, rs2, ins, rs2);
	*instr = ins;
	fprintf(stderr, "%08x: format-sb ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_U: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_u, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_u, rd, ins, rd);
	ins = bitfield_store(instr_u, imm31_12, ins, imm);
	*instr = ins;
	fprintf(stderr, "%08x: format-u ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_UJ: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_uj, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_uj, rd, ins, rd);
	ins = set_imm_uj(ins, imm);
	*instr = ins;
	fprintf(stderr, "%08x: format-uj ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 4;
	return 0;
    }
    case FORMAT_CR: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	ins = bitfield_store(instr_cr, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_cr, rs2, ins, rs2);
	ins = bitfield_store(instr_cr, rd, ins, rd);
	ins = bitfield_store(instr_cr, funct4, ins, isym->funct & 0xf);
	*instr = ins;
	fprintf(stderr, "%08x: format-cr ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 2;
	return 0;
    }

    case FORMAT_CI: {
	uint32_t* instr = (uint32_t*) &ctx->rt.mem[ctx->rt.waddr];
	uint32_t ins = 0;
	imm |= isym->value;
	ins = bitfield_store(instr_ci, opcode, ins, isym->opcode);
	ins = bitfield_store(instr_ci, rd, ins, rd);
	ins = bitfield_store(instr_ci, funct3, ins, isym->funct & 0x7);
	ins = bitfield_store(instr_ci, imm6_2, ins, imm & 0x1f);
	ins = bitfield_store(instr_ci, imm12, ins, (imm >> 5) & 1);
	*instr = ins;
	fprintf(stderr, "%08x: format-ci ins = %08x\n", ctx->rt.waddr, ins);
	ctx->rt.waddr += 2;
	return 0;
    }
    case FORMAT_CSS:
    case FORMAT_CIW:
    case FORMAT_CL:
    case FORMAT_CS:
    case FORMAT_CB:
    case FORMAT_CJ:
	fprintf(stderr, "%s:%d internal error, %s format %d not yet defined\n",
		ctx->filename, ctx->lineno, tokens[i].name, isym->format);	
	return -1;
    default:
	fprintf(stderr, "%s:%d internal error, %s format %d\n",
		ctx->filename, ctx->lineno, tokens[i].name, isym->format);	
	return -1;
    }

syntax_error:
    fprintf(stderr, "%s:%d: syntax error: %s\n", 
	    ctx->filename, ctx->lineno, ctx->linebuf);
    return -1;
}
