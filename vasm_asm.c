#include <ctype.h>
#include "vasm.h"

#define HARD_DEBUG

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

#define SYM_INSTR(nm) \
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = #nm }

#define SYM_INSTR_NAME(nm,nam)						\
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = nam }

symbol_t sym_instr_rv32i[] =
{
    SYM_INSTR(add),       // R
    SYM_INSTR(sub),       // R
    SYM_INSTR(sll),       // R
    SYM_INSTR(srl),       // R
    SYM_INSTR(sra),       // R
    SYM_INSTR(and),       // R
    SYM_INSTR(or),        // R
    SYM_INSTR(xor),       // R
    SYM_INSTR(slt),       // R
    SYM_INSTR(sltu),      // R
    SYM_INSTR(addi),      // I
    SYM_INSTR(slli),      // I
    SYM_INSTR(srli),      // I
    SYM_INSTR(srai),      // I
    SYM_INSTR(andi),      // I
    SYM_INSTR(ori),       // I
    SYM_INSTR(xori),      // I
    SYM_INSTR(slti),      // I
    SYM_INSTR(sltiu),     // I
    SYM_INSTR(lui),       // U
    SYM_INSTR(auipc),     // U
    SYM_INSTR(lb),        // I
    SYM_INSTR(lbu),       // I
    SYM_INSTR(lh),        // I
    SYM_INSTR(lhu),       // I
    SYM_INSTR(lw),        // I
    SYM_INSTR(sb),        // S
    SYM_INSTR(sh),        // S
    SYM_INSTR(sw),        // S
    SYM_INSTR(fence),     // I
    SYM_INSTR_NAME(fence_i,"fence.i"),   // I
    SYM_INSTR(beq),       // SB - Branch if equal
    SYM_INSTR(bne),       // SB - Branch if not equal
    SYM_INSTR(blt),       // SB - Branch if less than, 2's complement
    SYM_INSTR(bltu),      // SB - Branch if less than, unsigned
    SYM_INSTR(bge),       // SB - Branch if greater or equal, 2's complement
    SYM_INSTR(bgeu),      // SB - Branch if greater or equal, unsigned
    SYM_INSTR(jal),       // UJ - Jump and link
    SYM_INSTR(jalr),      // I  - Jump and link register
    SYM_INSTR(scall),     // I  - System call
    SYM_INSTR(sbreak),    // I  - Breakpoint
    SYM_INSTR(rdcycle),   // I  - 
    SYM_INSTR(rdcycleh),  // I  - 
    SYM_INSTR(rdtime),    // I  - 
    SYM_INSTR(rdtimeh),   // I  - 
    SYM_INSTR(rdinstret), // I  - 
    SYM_INSTR(rdinstreth), // I  - 
    
    // pseduo instructions
    SYM_INSTR(nop),        // addi x0,x0,0
    SYM_INSTR(j),          // jal x0,<imm>
};

void asm_init(symbol_table_t* symtab) 
{
    // link in static symbols above into symtab
    symbol_table_install_array(symtab, &sym_reg_abi[0], 
			       sizeof_array(sym_reg_abi));
    symbol_table_install_array(symtab, &sym_reg_xi[0],
			       sizeof_array(sym_reg_xi));
    symbol_table_install_array(symtab, &sym_instr_rv32i[0],
			       sizeof_array(sym_instr_rv32i));
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

int asm_imm5(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm12(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_imm20(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
{
    if (tokens[i].c == TOKEN_NUMBER)
	*imm = to_int(tokens[i].name);
    else
	return -1;
    return i+1;
}

int asm_reladdr12(vasm_ctx_t* ctx, token_t* tokens, int i, int* imm)
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
		instr_sb* instr = (instr_sb*) &ctx->rt.mem[link->addr];

		if (ctx->debug) {
		    fprintf(stderr, "resolve label %s @ %x = %d\n", 
			    tokens[0].name,link->addr,
			    sym->value - link->addr);
		}
		set_imm_sb(instr, sym->value - link->addr);
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
    
    if (tokens[i].c == TOKEN_SYMBOL) {
	symbol_t* isym = symbol_table_lookup(&ctx->symtab, tokens[i].name);
	// fprintf(stderr, "isym = %p\n", isym);
	if ((isym == NULL) || ((isym->flags & SYMBOL_FLAG_INSTR) == 0)) {
	    fprintf(stderr, "%s:%d unknown opcode %s\n", 
		    ctx->filename, ctx->lineno, tokens[i].name);
	    return -1;
	}
	i++;
	// fprintf(stderr, "isym->index = %d\n", isym->index);
	switch(isym->index) {
	case INSTR_add_SI: funct3 = FUNCT_ADD; goto op_r_format;
	case INSTR_sub_SI: funct3 = FUNCT_SUB; funct7 = 0x20; goto op_r_format;
	case INSTR_sll_SI: funct3 = FUNCT_SLL; goto op_r_format;
	case INSTR_slt_SI: funct3 = FUNCT_SLT; goto op_r_format;
	case INSTR_sltu_SI: funct3 = FUNCT_SLTU; goto op_r_format;
	case INSTR_xor_SI: funct3 = FUNCT_XOR; goto op_r_format;
	case INSTR_srl_SI: funct3 = FUNCT_SRL; goto op_r_format;
	case INSTR_sra_SI: funct3 = FUNCT_SRA; funct7 = 0x20; goto op_r_format;
	case INSTR_or_SI: funct3 = FUNCT_OR; goto op_r_format;
	case INSTR_and_SI: funct3 = FUNCT_AND; goto op_r_format;
	case INSTR_addi_SI: opcode = OPCODE_IMM; funct3 = FUNCT_ADDI; goto op_i_format;
	case INSTR_slli_SI: opcode = OPCODE_IMM; funct3 = FUNCT_SLLI; goto op_i_format;
	case INSTR_slti_SI: opcode = OPCODE_IMM; funct3 = FUNCT_SLTI; goto op_i_format;
	case INSTR_sltiu_SI: opcode = OPCODE_IMM; funct3 = FUNCT_SLTIU; goto op_i_format;
	case INSTR_xori_SI: opcode = OPCODE_IMM; funct3 = FUNCT_XORI; goto op_i_format;
	case INSTR_srli_SI: opcode = OPCODE_IMM; funct3 = FUNCT_SRLI; goto op_i_format;
	case INSTR_srai_SI: opcode = OPCODE_IMM; funct3 = FUNCT_SRAI; imm11_0 = 0x20; goto op_i_format;
	case INSTR_ori_SI: opcode = OPCODE_IMM; funct3 = FUNCT_ORI; goto op_i_format;
	case INSTR_andi_SI: opcode = OPCODE_IMM; funct3 = FUNCT_ANDI; goto op_i_format;
	case INSTR_lui_SI: opcode = OPCODE_LUI; goto op_u_format;
	case INSTR_auipc_SI: opcode = OPCODE_AUIPC; goto op_u_format;
	case INSTR_lb_SI: opcode = OPCODE_LOAD; funct3 = FUNCT_LB; goto op_i_format;
	case INSTR_lbu_SI: opcode = OPCODE_LOAD; funct3 = FUNCT_LBU; goto op_i_format;
	case INSTR_lh_SI: opcode = OPCODE_LOAD; funct3 = FUNCT_LH; goto op_i_format;
	case INSTR_lhu_SI: opcode = OPCODE_LOAD; funct3 = FUNCT_LHU; goto op_i_format;
	case INSTR_lw_SI: opcode = OPCODE_LOAD; funct3 = FUNCT_LW; goto op_i_format;
	case INSTR_sb_SI: opcode = OPCODE_STORE; funct3 = FUNCT_SB; goto op_s_format;
	case INSTR_sh_SI: opcode = OPCODE_STORE; funct3 = FUNCT_SH; goto op_s_format;
	case INSTR_sw_SI: opcode = OPCODE_STORE; funct3 = FUNCT_SW; goto op_s_format;
	case INSTR_fence_SI: opcode = OPCODE_FENCE; funct3 = FUNCT_FENCE; goto op_i_format;
	case INSTR_fence_i_SI: opcode = OPCODE_FENCE; funct3 = FUNCT_FENCE_I; goto op_i_format;
	case INSTR_beq_SI: funct3 = FUNCT_BEQ; goto op_sb_format;
	case INSTR_bne_SI: funct3 = FUNCT_BNE; goto op_sb_format;
	case INSTR_blt_SI: funct3 = FUNCT_BLT; goto op_sb_format;
	case INSTR_bltu_SI: funct3 = FUNCT_BLTU; goto op_sb_format;
	case INSTR_bge_SI: funct3 = FUNCT_BGE; goto op_sb_format;
	case INSTR_bgeu_SI: funct3 = FUNCT_BGEU; goto op_sb_format;
	case INSTR_jal_SI: opcode = OPCODE_JAL; goto op_uj_format;
	case INSTR_jalr_SI: opcode = OPCODE_JALR; goto op_i_format;
	case INSTR_scall_SI:  opcode = OPCODE_SYS; imm11_0=0x000; goto op_i_format;
	case INSTR_sbreak_SI: opcode = OPCODE_SYS; imm11_0=0x001; goto op_i_format;
	case INSTR_rdcycle_SI: opcode = OPCODE_SYS; imm11_0=0xc00; goto op_i_format;
	case INSTR_rdcycleh_SI: opcode = OPCODE_SYS; imm11_0=0xc80; goto op_i_format;
	case INSTR_rdtime_SI: opcode = OPCODE_SYS; imm11_0=0xc01; goto op_i_format;
	case INSTR_rdtimeh_SI: opcode = OPCODE_SYS; imm11_0=0xc81; goto op_i_format;
	case INSTR_rdinstret_SI: opcode = OPCODE_SYS; imm11_0=0xc02; goto op_i_format;
	case INSTR_rdinstreth_SI: opcode = OPCODE_SYS; imm11_0=0xc82; goto op_i_format;
       // Pseudo opcodes
	case INSTR_nop_SI: {  // addi x0,x0,0
	    instr_i* instr = (instr_i*) &ctx->rt.mem[ctx->rt.waddr];

	    instr->opcode = OPCODE_IMM;
	    instr->rd = 0;
	    instr->funct3 = FUNCT_ADDI;
	    instr->rs1 = 0;
	    instr->imm11_0 = 0;
	    ctx->rt.waddr += 4;
	    return 0;
	}

	case INSTR_j_SI: {   // jal x0,<imm>
	    instr_uj* instr = (instr_uj*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;
	    if ((i = asm_reladdr20(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    i++;
	    instr->opcode = OPCODE_JAL;
	    instr->rd = 0;  // x0
	    set_imm_uj(instr, imm);
	    ctx->rt.waddr += 4;
	    return 0;
	}

	default:
	    fprintf(stderr, "%s:%d unknown opcode %s\n", 
		    ctx->filename, ctx->lineno, isym->name);
	    return -1;
	}

    op_r_format: {
	    int rs1, rs2, rd;
	    instr_r* instr = (instr_r*) &ctx->rt.mem[ctx->rt.waddr];

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

    op_i_format: {
	    int rs1 = 0, rd = 0;
	    int imm = 0;
	    instr_i* instr = (instr_i*) &ctx->rt.mem[ctx->rt.waddr];

	    switch(opcode) {
	    case OPCODE_LOAD:
	    case OPCODE_JALR:
		if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		    goto syntax_error;
		if (tokens[i].c != ',')
		    goto syntax_error;
		i++;
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

	    case OPCODE_FENCE:
		if (funct3 == FUNCT_FENCE) {
		    int pred, succ; // fence pred (iorw), succ (iorw)
		    if ((i = asm_iorw(ctx,tokens,i,&pred)) < 0)
			goto syntax_error;
		    if (tokens[i].c != ',')
			goto syntax_error;
		    i++;
		    if ((i = asm_iorw(ctx,tokens,i,&succ)) < 0)
			goto syntax_error;
		    imm = (pred << 4) | succ;
		}
		break;

	    case OPCODE_SYS:
		if (imm11_0 > 0x001) {
		    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
			goto syntax_error;
		}
		break;

	    default:
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
		if ((funct3 == FUNCT_SLLI) || (funct3 == FUNCT_SRAI)) {
		    if ((i = asm_imm5(ctx,tokens,i,&imm)) < 0)
			goto syntax_error;
		}
		else {
		    if ((i = asm_imm12(ctx,tokens,i,&imm)) < 0)
			goto syntax_error;
		}
		break;
	    }
	    instr->opcode = opcode;
	    instr->rd = rd;
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->imm11_0 = imm | imm11_0;
	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_s_format: {
	    int rs1, rs2;
	    instr_s* instr = (instr_s*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

	    if ((i = asm_reg(ctx,tokens,i,&rs2)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;

	    if (tokens[i].c == '(') {
		imm = 0;
		i++;
	    }
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

	    instr->opcode = opcode;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    instr->funct3 = funct3;
	    set_imm_s(instr, imm);
	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_sb_format: {
	    int rs1, rs2;
	    instr_sb* instr = (instr_sb*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

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
	    if ((i = asm_reladdr12(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    i++;
	    instr->opcode = OPCODE_BRANCH;
	    set_imm_sb(instr, imm);
	    instr->funct3 = funct3;
	    instr->rs1 = rs1;
	    instr->rs2 = rs2;
	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_u_format: {
	    int rd;
	    instr_u* instr = (instr_u*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

	    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_imm20(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;

	    instr->opcode = opcode;
	    instr->rd = rd;
	    instr->imm31_12 = imm;
	    ctx->rt.waddr += 4;
	    return 0;
	}

    op_uj_format: {
	    int rd;
	    instr_uj* instr = (instr_uj*) &ctx->rt.mem[ctx->rt.waddr];
	    int imm;

	    if ((i = asm_reg(ctx,tokens,i,&rd)) < 0)
		goto syntax_error;
	    if (tokens[i].c != ',')
		goto syntax_error;
	    i++;
	    if ((i = asm_reladdr20(ctx,tokens,i,&imm)) < 0)
		goto syntax_error;
	    i++;

	    instr->opcode = opcode;
	    instr->rd = rd;
	    set_imm_uj(instr, imm);
	    ctx->rt.waddr += 4;
	    return 0;
	}
    }

syntax_error:
    fprintf(stderr, "%s:%d: syntax error: %s\n", 
	    ctx->filename, ctx->lineno, ctx->linebuf);
    return -1;
}
