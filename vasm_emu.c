
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
uint32_t v64c_inflate(void* p, uint32_t ins)
{
    switch(bitfield_fetch(instr_c,opcode,ins)) {
    case OPCODE_C0:
	switch(bitfield_fetch(instr_c,funct3,ins)) {
	case FUNCT_C_ADDI4SPN:
	case FUNCT_C_FLD:
	case FUNCT_C_FLW:
//	case FUNCT_C_LD:
	case FUNCT_C_FSD:
	case FUNCT_C_SW:
//	case FUNCT_C_SD:
	    break;
	}
	break;
    case OPCODE_C1:
	switch(bitfield_fetch(instr_c,funct3,ins)) {
	case FUNCT_C_NOP:
//	case FUNCT_C_ADDI:
	case FUNCT_C_ADDIW:
	case FUNCT_C_LI:
//	case FUNCT_C_ADDI16SP:
	case FUNCT_C_LUI:
	case FUNCT_C_SRLI:
//	case FUNCT_C_SRAI:
//	case FUNCT_C_ANDI:
//	case FUNCT_C_SUB:
//	case FUNCT_C_XOR:
//	case FUNCT_C_OR:
//	case FUNCT_C_AND:
//	case FUNCT_C_SUBW:
//	case FUNCT_C_ADDW:
	case FUNCT_C_J:
	case FUNCT_C_BEQZ:
	case FUNCT_C_BNEZ:
	    break;
	}
	break;
    case OPCODE_C2:
	switch(bitfield_fetch(instr_c,funct3,ins)) {
	case FUNCT_C_SLLI:
	case FUNCT_C_FLDSP:
	case FUNCT_C_LWSP:
	case FUNCT_C_LDSP:
	case FUNCT_C_JR:
//	case FUNCT_C_MV:
//	case FUNCT_C_EBREAK:
//	case FUNCT_C_JALR:
//	case FUNCT_C_ADD:
	    switch(bitfield_fetch(instr_cr,funct4,ins)) {
	    case 8:
		break;
	    case 9:
		if ((bitfield_fetch(instr_cr,rs2,ins) != 0) &&
		    (bitfield_fetch(instr_cr,rd,ins) != 0)) {
		    uint32_t ins1 = 0;
		    ins1 = bitfield_store(instr_r, opcode, ins1, OPCODE_ARITH);
		    ins1 = bitfield_store(instr_r, rd, ins1,
					  bitfield_fetch(instr_cr,rd,ins));
		    ins1 = bitfield_store(instr_r, rs1, ins1, 0);
		    ins1 = bitfield_store(instr_r, rs2, ins1,
					  bitfield_fetch(instr_cr,rs2,ins));
		    ins1 = bitfield_store(instr_r, funct3, ins1, FUNCT_ADD);
		    ins1 = bitfield_store(instr_r, funct7, ins1, 0);
		    return ins1;
		}
		break;
	    }
	    break;
	case FUNCT_C_FSDSP:
	case FUNCT_C_SWSP:
	case FUNCT_C_SDSP:
	    break;
	}
	break;
    }
    return 0;
}
#endif    

// when implementing compressed instructions are 16bit aligned
unsigned_t emu(vasm_rt_t* ctx,unsigned_t pc, void* mem)
{
    void* p = (void*) ((uint8_t*)mem + pc);
    int rs1, rs2, rd, imm;
#if BYTE_ORDER == LITTLE_ENDIAN
    uint32_t ins = ((uint16_t*)p)[0];
#else
    uint32_t ins = ((uint8_t*)p)[0] | ((((uint8_t*)p)[1]) << 8);
#endif

#if defined(RV32C) || defined(RV64C)
    if (OPCODE_IS_16BIT(ins))
	ins = v64c_inflate(p, ins);
    else
#endif
    {
#if BYTE_ORDER == LITTLE_ENDIAN
	ins |= (((uint16_t*)p)[1] << 16);
#else
	ins |= (((uint8_t*)p)[2] | ((((uint8_t*)p)[3]) << 8)) << 16;
#endif
    }

    switch(bitfield_fetch(instr_t,opcode,ins)) {
    case OPCODE_ARITH: {  // R-type
	switch(bitfield_fetch(instr_r,funct7,ins)) {
	case 0x00:
	    rd = bitfield_fetch(instr_r,rd,ins);
	    rs1 = bitfield_fetch(instr_r,rs1,ins);
	    rs2 = bitfield_fetch(instr_r,rs2,ins);
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_ADD:
		wrr(ctx,rd,rdr(ctx,rs1) + rdr(ctx,rs2));
		break;
	    case FUNCT_SLL:
		wrr(ctx,rd,rdr(ctx,rs1) << rdr(ctx,rs2));
		break;
	    case FUNCT_SLT:
		wrr(ctx,rd, (rdr(ctx,rs1) < rdr(ctx,rs2)));
		break;
	    case FUNCT_SLTU:
		wrr(ctx,rd, ((ureg_t)rdr(ctx,rs1) < (ureg_t)rdr(ctx,rs2)));
		break;
	    case FUNCT_XOR:
		wrr(ctx,rd,rdr(ctx,rs1) ^ rdr(ctx,rs2));
		break;
	    case FUNCT_SRL:
		wrr(ctx,rd,(ureg_t)rdr(ctx,rs1) >> rdr(ctx,rs2));
		break;
	    case FUNCT_OR:
		wrr(ctx,rd,rdr(ctx,rs1) | rdr(ctx,rs2));
		break;
	    case FUNCT_AND:
		wrr(ctx,rd,rdr(ctx,rs1) & rdr(ctx,rs2));
		break;
	    }
	    break;
	case 0x01:
	    rd = bitfield_fetch(instr_r,rd,ins);
	    rs1 = bitfield_fetch(instr_r,rs1,ins);
	    rs2 = bitfield_fetch(instr_r,rs2,ins);
#if defined(RV32M)
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_MUL:
		wrr(ctx,rd,rdr(ctx,rs1) * rdr(ctx,rs2));
		break;
	    case FUNCT_MULH: {
		dreg_t r = rdr(ctx,rs1);
		r *= rdr(ctx,rs2);
		wrr(ctx,rd, r>>XLEN);
		break;
	    }
	    case FUNCT_MULHSU: {
		dreg_t r = rdr(ctx,rs1);
		r *= (ureg_t)rdr(ctx,rs2);
		wrr(ctx,rd, r>>XLEN);
		break;
	    }
	    case FUNCT_MULHU: {
		udreg_t r = (ureg_t)rdr(ctx,rs1);
		r *= (ureg_t)rdr(ctx,rs2);
		wrr(ctx,rd, r>>XLEN);
		break;
	    }
	    case FUNCT_DIV: {
		reg_t divisor = rdr(ctx,rs2);
		if (divisor == 0)
		    wrr(ctx,rd,-1);
		else {
		    reg_t dividend = rdr(ctx,rs1);
		    if (dividend == -(1 << (XLEN-1)))
			wrr(ctx,rd,dividend);
		    else
			wrr(ctx,rd,dividend / divisor);
		}
		break;
	    }
	    case FUNCT_DIVU: {
		ureg_t divisor = (ureg_t)rdr(ctx,rs2);
		if (divisor == 0)
		    wrr(ctx,rd,-1);
		else {
		    ureg_t dividend = (ureg_t) rdr(ctx,rs1);
		    wrr(ctx,rd,dividend / divisor);
		}
		break;
	    }
	    case FUNCT_REM: {
		reg_t divisor = rdr(ctx,rs2);
		reg_t dividend = rdr(ctx,rs1);
		if (divisor == 0)
		    wrr(ctx,rd,dividend);
		else {
		    if (dividend == -(1 << (XLEN-1)))
			wrr(ctx,rd,0);
		    else
			wrr(ctx,rd,dividend % divisor);
		}
		break;
	    }
	    case FUNCT_REMU: {
		ureg_t divisor = rdr(ctx,rs2);
		if (divisor == 0)
		    wrr(ctx,rd,0);
		else {
		    ureg_t dividend = rdr(ctx,rs1);
		    wrr(ctx,rd,dividend % divisor);
		}
		break;
	    }
	    default:
		break;
	    }
#endif
	    break;
	case 0x20:
	    rd = bitfield_fetch(instr_r,rd,ins);
	    rs1 = bitfield_fetch(instr_r,rs1,ins);
	    rs2 = bitfield_fetch(instr_r,rs2,ins);
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_SUB:
		wrr(ctx,rd,rdr(ctx,rs1) - rdr(ctx,rs2));
		break;
	    case FUNCT_SRA:
		wrr(ctx,rd,rdr(ctx,rs1) >> rdr(ctx,rs2));
		break;
	    default:
		break;
	    }
	    break;
	default:
	    break;
	}			    
	break;
    }
    case OPCODE_IMM: {   // I-type
	rd = bitfield_fetch(instr_i,rd,ins);
	rs1 = bitfield_fetch(instr_i,rs1,ins);
	imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	switch(bitfield_fetch(instr_i,funct3,ins)) {
	case FUNCT_ADDI:
	    wrr(ctx,rd,rdr(ctx,rs1) + imm);
	    break;
	case FUNCT_SLLI:
	    wrr(ctx,rd,rdr(ctx,rs1) << imm);
	    break;
	case FUNCT_SLTI:
	    wrr(ctx,rd,(rdr(ctx,rs1) < imm));
	    break;
	case FUNCT_SLTIU:
	    wrr(ctx,rd,((ureg_t)rdr(ctx,rs1) < (ureg_t)imm));
	    break;
	case FUNCT_XORI:
	    wrr(ctx,rd,rdr(ctx,rs1) ^ imm);
	    break;
	case FUNCT_SRLI:  // imm11_0 = 0000000
     // case FUNCT_SRAI:   // imm11_0 = 0100000
	    if (imm == 0x00)
		wrr(ctx,rd, (ureg_t)rdr(ctx,rs1) >> imm);
	    else if (imm == 0x20)
		wrr(ctx,rd,rdr(ctx,rs1) >> imm);
	    break;
	case FUNCT_ORI:
	    wrr(ctx,rd,rdr(ctx,rs1) | imm);
	    break;
	case FUNCT_ANDI:
	    wrr(ctx,rd,rdr(ctx,rs1) & imm);
	    break;
	}
	break;	
    }

    case OPCODE_LOAD: { // I-type
	void* addr;
	rd = bitfield_fetch(instr_i,rd,ins);
	rs1 = bitfield_fetch(instr_i,rs1,ins);
	imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	addr = (void*) ((uint8_t*)mem + rdr(ctx,rs1)+imm);
	switch(bitfield_fetch(instr_i,funct3,ins)) {
	case FUNCT_LB:
	    wrr(ctx,rd,*((int8_t*)addr));
	    break;
	case FUNCT_LH:
	    wrr(ctx,rd,*((int16_t*)addr));
	    break;
	case FUNCT_LW:
	    wrr(ctx,rd,*((int32_t*)addr));
	    break;
	case FUNCT_LBU:
	    wrr(ctx,rd,*((uint8_t*)addr));
	    break;
	case FUNCT_LHU:
	    wrr(ctx,rd,*((uint16_t*)addr));
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
	void* addr;
	rs1 = bitfield_fetch(instr_s,rs1,ins);
	rs2 = bitfield_fetch(instr_s,rs2,ins);
	addr = (void*) ((uint8_t*)mem+rdr(ctx,rs1)+get_imm_s(ins));
	switch(bitfield_fetch(instr_s,funct3,ins)) {
	case FUNCT_SB:
	    *((uint8_t*)addr) = rdr(ctx,rs2);
	    break;
	case FUNCT_SH:
	    *((uint16_t*)addr) = rdr(ctx,rs2);
	    break;
	case FUNCT_SW:
	    *((uint32_t*)addr) = rdr(ctx,rs2);
	    break;
	default:
	    break;
	}
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	rs1 = bitfield_fetch(instr_sb,rs1,ins);
	rs2 = bitfield_fetch(instr_sb,rs2,ins);
	switch(bitfield_fetch(instr_sb,funct3,ins)) {
	case FUNCT_BEQ:
	    if (rdr(ctx,rs1) == rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	case FUNCT_BNE:
	    if (rdr(ctx,rs1) != rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	case FUNCT_BLT:
	    if (rdr(ctx,rs1) < rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	case FUNCT_BGE:
	    if (rdr(ctx,rs1) >= rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	case FUNCT_BLTU:
	    if ((ureg_t)rdr(ctx,rs1) < (ureg_t)rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	case FUNCT_BGEU:
	    if ((ureg_t)rdr(ctx,rs1) >= (ureg_t)rdr(ctx,rs2))
		return pc + get_imm_sb(ins);
	    break;
	}
	break;
    }
    case OPCODE_LUI:  { // U-type
	rd = bitfield_fetch(instr_u,rd,ins);
	imm = bitfield_fetch(instr_u,imm31_12,ins);
	wrr(ctx,rd,(imm << 12));
	break;
    }
    case OPCODE_AUIPC: { // U-type
	rd = bitfield_fetch(instr_u,rd,ins);
	imm = bitfield_fetch(instr_u,imm31_12,ins);
	wrr(ctx,rd, ctx->pc + (imm << 12));
	break;
    }

    case OPCODE_JALR: {
	rs1 = bitfield_fetch(instr_i,rs1,ins);
	rd = bitfield_fetch(instr_i,rd,ins);
	imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	reg_t pc1;
	wrr(ctx,rd,ctx->pc + 4);
	pc1 = (imm + rdr(ctx,rs1)) & ~1;
	return pc1;
    }
	
    case OPCODE_JAL: {   // Uj-type
	rd = bitfield_fetch(instr_uj,rd,ins);	
	wrr(ctx,rd,ctx->pc + 4);
	return pc + get_imm_uj(ins);
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
