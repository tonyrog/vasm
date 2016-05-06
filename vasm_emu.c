
#include "vasm.h"
#include "vasm_rt.h"

static inline reg_t rdr(vasm_rt_t* ctx, int r)
{
    return (r == 0) ? 0 : ctx->reg[r];
}

static inline void wrr(vasm_rt_t* ctx, int r, reg_t val)
{
    if (r != 0)
	ctx->reg[r] = val;
}

// inflate compressed instruction if needed
// if data is to be inflated put data in imem

#if defined(RV32C) || defined(RV64C)
static void* inflate(unsigned_t addr, void* mem, void* imem)
{
    void* p = (void*) ((uint8_t*)mem + addr);
    switch(((instr_c*)p)->opcode) {
    case 0:
	break;
    case 1:
	break;
    case 2: {
	instr_cr* instr = (instr_cr*) p;
	switch(instr->funct4) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	    if ((instr->rs2 != 0) && (instr->rd != 0)) {
		instr_r* xinstr = (instr_r*) imem;
		xinstr->opcode = OPCODE_ARITH;
		xinstr->rd = instr->rd;
		xinstr->rs1 = 0;  // zero
		xinstr->rs2 = instr->rs2;
		xinstr->funct3 = FUNCT_ADD;
		xinstr->funct7 = 0x00;
		return xinstr;
	    }
	    break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	    break;
	}
    }
    default:
	break;
    }
    return p;
}
#endif    

// when implementing compressed instructions are 16bit aligned
unsigned_t emu(vasm_rt_t* ctx,unsigned_t pc, void* mem)
{
#if defined(RV32C) || defined(RV64C)
    instr_t temp;
    void* p = inflate(pc, mem, &temp);
#else
    void* p = (void*) ((uint8_t*)mem + pc);
#endif
    switch(((instr_t*)p)->opcode) {
    case OPCODE_ARITH: {  // R-type
	instr_r* ip = (instr_r*) p;
	switch(ip->funct3) {
	case FUNCT_ADD:  // funct7 = 0000000
     // case FUNCT_SUB:  // funct7 = 0100000
	    if (ip->funct7 == 0x00)
		wrr(ctx,ip->rd,rdr(ctx,ip->rs1) + rdr(ctx,ip->rs2));
	    else if (ip->funct7 == 0x20)
		wrr(ctx,ip->rd,rdr(ctx,ip->rs1) - rdr(ctx,ip->rs2));
	    break;
	case FUNCT_SLL:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) << rdr(ctx,ip->rs2));
	    break;
	case FUNCT_SLT:
	    if ((int32_t)rdr(ctx,ip->rs1) < (int32_t)rdr(ctx,ip->rs2))
		wrr(ctx,ip->rd,1);
	    break;
	case FUNCT_SLTU:
	    if (rdr(ctx,ip->rs1) < rdr(ctx,ip->rs2))
		wrr(ctx,ip->rd,1);
	    break;
	case FUNCT_XOR:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) ^ rdr(ctx,ip->rs2));
	    break;
	case FUNCT_SRL:   // funct7 = 0000000
     // case FUNCT_SRA:   // funct7 = 0100000
	    if (ip->funct7 == 0x00)
		wrr(ctx,ip->rd,rdr(ctx,ip->rs1) >> rdr(ctx,ip->rs2));
	    else if (ip->funct7 == 0x20)
		wrr(ctx,ip->rd,(int32_t)rdr(ctx,ip->rs1) >> rdr(ctx,ip->rs2));
	    break;
	case FUNCT_OR:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) | rdr(ctx,ip->rs2));
	    break;
	case FUNCT_AND:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) & rdr(ctx,ip->rs2));
	    break;
	}
	break;
    }
    case OPCODE_IMM: {   // I-type
	instr_i* ip = (instr_i*) p;
	switch(ip->funct3) {
	case FUNCT_ADDI:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) + ip->imm11_0);
	    break;
	case FUNCT_SLLI:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) << ip->imm11_0);
	    break;
	case FUNCT_SLTI:
	    if ((int32_t)rdr(ctx,ip->rs1) < ip->imm11_0)
		wrr(ctx,ip->rd,1);
	    break;
	case FUNCT_SLTIU:
	    if (rdr(ctx,ip->rs1) < (unsigned)ip->imm11_0)
		wrr(ctx,ip->rd,1);
	    break;
	case FUNCT_XORI:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) ^ ip->imm11_0);
	    break;
	case FUNCT_SRLI:  // imm11_0 = 0000000
     // case FUNCT_SRAI:   // imm11_0 = 0100000
	    if (ip->imm11_0 == 0x00)
		wrr(ctx,ip->rd, rdr(ctx,ip->rs1) >> ip->imm11_0);
	    else if (ip->imm11_0 == 0x20)
		wrr(ctx,ip->rd,(int32_t)rdr(ctx,ip->rs1) >> ip->imm11_0);
	    break;
	case FUNCT_ORI:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) | ip->imm11_0);
	    break;
	case FUNCT_ANDI:
	    wrr(ctx,ip->rd,rdr(ctx,ip->rs1) & ip->imm11_0);
	    break;
	}
	break;	
    }

    case OPCODE_LOAD: { // I-type
	instr_i* ip = (instr_i*) p;
	void* addr = (void*) ((uint8_t*)mem + rdr(ctx,ip->rs1)+ip->imm11_0);
	switch(ip->funct3) {
	case FUNCT_LB:
	    wrr(ctx,ip->rd, *((int8_t*)addr));
	    break;
	case FUNCT_LH:
	    wrr(ctx,ip->rd,*((int16_t*)addr));
	    break;
	case FUNCT_LW:
	    wrr(ctx,ip->rd,*((int32_t*)addr));
	    break;
	case FUNCT_LBU:
	    wrr(ctx,ip->rd,*((uint8_t*)addr));
	    break;
	case FUNCT_LHU:
	    wrr(ctx,ip->rd,*((uint16_t*)addr));
	    break;
	default:
	    break;
	}
	break;
    }

    case OPCODE_FENCE: // I-type
	break;

    case OPCODE_SYS:   // I-type
	break;

    case OPCODE_STORE: {  // S-type
	instr_s* ip = (instr_s*) p;
	void* addr = (void*) ((uint8_t*)mem+rdr(ctx,ip->rs1)+get_imm_s(ip));
	switch(ip->funct3) {
	case FUNCT_SB:
	    *((uint8_t*)addr) = rdr(ctx,ip->rs2);
	    break;
	case FUNCT_SH:
	    *((uint16_t*)addr) = rdr(ctx,ip->rs2);
	    break;
	case FUNCT_SW:
	    *((uint32_t*)addr) = rdr(ctx,ip->rs2);
	    break;
	default:
	    break;
	}
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	instr_sb* ip = (instr_sb*) p;
	switch(ip->funct3) {
	case FUNCT_BEQ:
	    if (rdr(ctx,ip->rs1) == rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	case FUNCT_BNE:
	    if (rdr(ctx,ip->rs1) != rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	case FUNCT_BLT:
	    if (rdr(ctx,ip->rs1) < rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	case FUNCT_BGE:
	    if (rdr(ctx,ip->rs1) >= rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	case FUNCT_BLTU:
	    if ((ureg_t)rdr(ctx,ip->rs1) < (ureg_t)rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	case FUNCT_BGEU:
	    if ((ureg_t)rdr(ctx,ip->rs1) >= (ureg_t)rdr(ctx,ip->rs2))
		return pc + get_imm_sb(ip);
	    break;
	}
	break;
    }
    case OPCODE_LUI:  { // U-type
	instr_u* ip = (instr_u*) p;
	wrr(ctx,ip->rd,(ip->imm31_12 << 12));
	break;
    }
    case OPCODE_AUIPC: { // U-type
	instr_u* ip = (instr_u*) p;
	wrr(ctx,ip->rd, ctx->pc + (ip->imm31_12 << 12));
	break;
    }

    case OPCODE_JALR: {
	instr_i* ip = (instr_i*) p;
	reg_t pc1;
	wrr(ctx,ip->rd,ctx->pc + 4);
	pc1 = (ip->imm11_0 + rdr(ctx,ip->rs1)) & ~1;
	return pc1;
    }
	
    case OPCODE_JAL: {   // Uj-type
	instr_uj* ip = (instr_uj*) p;
	wrr(ctx,ip->rd,ctx->pc + 4);
	return pc + get_imm_uj(ip);
    }

    }
    return pc+4;
}

void dump_regs(FILE* f, vasm_rt_t* ctx)
{
    int i;

    for (i = 0; i < NUM_REGISTERS; i++) {
	fprintf(f, "%s = %08x", register_abi_name(i), ctx->reg[i]);
	if ((i & 3) == 3) 
	    fprintf(f, "\n");
	else
	    fprintf(f, "    ");
    }
}

void run(FILE* f, symbol_table_t* symtab, vasm_rt_t* ctx, unsigned_t pc)
{
    dump_regs(f, ctx);
    while(pc < ctx->waddr) {
	disasm_instr(f, symtab, pc, ctx->mem);
	pc = emu(ctx, pc, ctx->mem);
	dump_regs(f, ctx);
    }
}
