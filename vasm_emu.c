
#include "vasm.h"
#include "vasm_emu.h"

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
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_ADD: RV32I_ADD(ctx, ins); break;
	    case FUNCT_SLL: RV32I_SLL(ctx, ins); break;
	    case FUNCT_SLT: RV32I_SLT(ctx, ins); break;
	    case FUNCT_SLTU: RV32I_SLTU(ctx, ins); break;
	    case FUNCT_XOR: RV32I_XOR(ctx, ins); break;
	    case FUNCT_SRL: RV32I_SRL(ctx, ins); break;
	    case FUNCT_OR: RV32I_OR(ctx, ins); break;
	    case FUNCT_AND: RV32I_AND(ctx, ins); break;
	    default: goto illegal;
	    }
	    break;
	case 0x01:
#if defined(RV32M)
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_MUL: RV32M_MUL(ctx,ins); break;
	    case FUNCT_MULH: RV32M_MULH(ctx,ins); break;
	    case FUNCT_MULHSU: RV32M_MULHSU(ctx,ins); break;
	    case FUNCT_MULHU: RV32M_MULHU(ctx,ins); break;
	    case FUNCT_DIV: RV32M_DIV(ctx,ins); break;
	    case FUNCT_DIVU: RV32M_DIVU(ctx,ins); break;
	    case FUNCT_REM: RV32M_REM(ctx,ins); break;
	    case FUNCT_REMU: RV32M_REMU(ctx,ins); break;
	    default: goto illegal;
	    }
#endif
	    break;
	case 0x20:
	    switch(bitfield_fetch(instr_r,funct3,ins)) {
	    case FUNCT_SUB: RV32I_SUB(ctx,ins); break;
	    case FUNCT_SRA: RV32I_SRA(ctx,ins); break;
	    default: goto illegal;
	    }
	    break;
	default: goto illegal;
        }			    
	break;
    }
    case OPCODE_IMM: {   // I-type
	rd = bitfield_fetch(instr_i,rd,ins);
	rs1 = bitfield_fetch(instr_i,rs1,ins);
	imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	switch(bitfield_fetch(instr_i,funct3,ins)) {
	case FUNCT_ADDI: rv32i_addi(ctx,rd,rs1,imm); break;
	case FUNCT_SLLI: rv32i_slli(ctx,rd,rs1,imm); break;
	case FUNCT_SLTI: rv32i_slti(ctx,rd,rs1,imm); break;
	case FUNCT_SLTIU: rv32i_sltiu(ctx,rd,rs1,imm); break;
	case FUNCT_XORI: rv32i_xori(ctx,rd,rs1,imm); break;
	case FUNCT_SRLI:  // imm11_0 = 0000000
     // case FUNCT_SRAI:   // imm11_0 = 0100000
	    switch((imm >> 5) & 0x7f) {
	    case 0x00: rv32i_srli(ctx,rd,rs1,imm&0x1f); break;
	    case 0x20: rv32i_srai(ctx,rd,rs1,imm&0x1f); break;
	    default: goto illegal;
	    }
	    break;
	case FUNCT_ORI: rv32i_ori(ctx,rd,rs1,imm); break;
	case FUNCT_ANDI: rv32i_andi(ctx,rd,rs1,imm); break;
	default: goto illegal;
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
	case FUNCT_LB: rv32i_lb(ctx,rd,addr); break;
	case FUNCT_LH: rv32i_lh(ctx,rd,addr); break;
	case FUNCT_LW: rv32i_lw(ctx,rd,addr); break;
	case FUNCT_LBU: rv32i_lbu(ctx,rd,addr); break;
	case FUNCT_LHU: rv32i_lhu(ctx,rd,addr); break;
	default: goto illegal;
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
	case FUNCT_SB: rv32i_sb(ctx,rs2,addr); break;
	case FUNCT_SH: rv32i_sh(ctx,rs2,addr); break;
	case FUNCT_SW: rv32i_sw(ctx,rs2,addr); break;
	default: goto illegal;
	}
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	rs1 = bitfield_fetch(instr_sb,rs1,ins);
	rs2 = bitfield_fetch(instr_sb,rs2,ins);
	imm = get_imm_sb(ins);
	switch(bitfield_fetch(instr_sb,funct3,ins)) {
	case FUNCT_BEQ:
	    if (rv32i_beq(ctx,rs1,rs2))
		return pc+imm;
	    break;
	case FUNCT_BNE: 
	    if (rv32i_bne(ctx,rs1,rs2))
		return pc+imm;
	    break;
	case FUNCT_BLT: 
	    if (rv32i_blt(ctx,rs1,rs2))
		return pc+imm;
	    break;
	case FUNCT_BGE: 
	    if (rv32i_bge(ctx,rs1,rs2))
		return pc+imm;
	    break;
	case FUNCT_BLTU:
	    if (rv32i_bltu(ctx,rs1,rs2))
		return pc+imm;
	    break;
	case FUNCT_BGEU:
	    if (rv32i_bgeu(ctx,rs1,rs2))
		return pc+imm;
	    break;
	default: 
	    goto illegal;
	}
	break;
    }
    case OPCODE_LUI:  { // U-type
	rd = bitfield_fetch(instr_u,rd,ins);
	imm = bitfield_fetch(instr_u,imm31_12,ins);
	rv32i_lui(ctx,rd,imm);
	break;
    }
    case OPCODE_AUIPC: { // U-type
	rd = bitfield_fetch(instr_u,rd,ins);
	imm = bitfield_fetch(instr_u,imm31_12,ins);
	rv32i_auipc(ctx,rd,imm);
	break;
    }

    case OPCODE_JALR: {
	rs1 = bitfield_fetch(instr_i,rs1,ins);
	rd = bitfield_fetch(instr_i,rd,ins);
	imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	return rv32i_jalr(ctx, rd, rs1, imm);
    }
	
    case OPCODE_JAL: {   // Uj-type
	rd = bitfield_fetch(instr_uj,rd,ins);	
	imm = get_imm_uj(ins);
	return pc+rv32i_jal(ctx,rd,imm);
    }
    default: goto illegal;
    }
    return pc+4;
illegal:
    fprintf(stderr, "%08x: illegal instruction %08x \r\n", pc, ins);
    return (unsigned_t) -1;
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
