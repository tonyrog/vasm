
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
	switch(bitfield_fetch(instr_i,funct3,ins)) {
	case FUNCT_ADDI: RV32I_ADDI(ctx,ins); break;
	case FUNCT_SLLI: RV32I_SLLI(ctx,ins); break;
	case FUNCT_SLTI: RV32I_SLTI(ctx,ins); break;
	case FUNCT_SLTIU: RV32I_SLTIU(ctx,ins); break;
	case FUNCT_XORI: RV32I_XORI(ctx,ins); break;
	case FUNCT_SRLI:  // imm11_0 = 0000000
     // case FUNCT_SRAI:   // imm11_0 = 0100000
	    switch((bitfield_fetch(instr_i,imm11_0,ins) >> 5) & 0x7f) {
	    case 0x00: RV32I_SRLI(ctx,ins); break;
	    case 0x20: RV32I_SRAI(ctx,ins); break;
	    default: goto illegal;
	    }
	    break;
	case FUNCT_ORI: RV32I_ORI(ctx,ins); break;
	case FUNCT_ANDI: RV32I_ANDI(ctx,ins); break;
	default: goto illegal;
	}
	break;	
    }

    case OPCODE_LOAD: { // I-type
	switch(bitfield_fetch(instr_i,funct3,ins)) {
	case FUNCT_LB: RV32I_LB(ctx,ins); break;
	case FUNCT_LH: RV32I_LH(ctx,ins); break;
	case FUNCT_LW: RV32I_LW(ctx,ins); break;
	case FUNCT_LBU: RV32I_LBU(ctx,ins); break;
	case FUNCT_LHU: RV32I_LHU(ctx,ins); break;
	default: goto illegal;
	}
	break;
    }

    case OPCODE_FENCE: // I-type
	break;

    case OPCODE_SYS: {   // I-type
	// int _rd = bitfield_fetch(instr_i,rd,(ins));
	// int _rs1 = bitfield_fetch(instr_i,rs1,(ins));
	int _imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	// RV32I_DECODE_I;
	if (_imm == 0)
	    rv32i_ecall(ctx);
	break;
    }

    case OPCODE_STORE: {  // S-type
	switch(bitfield_fetch(instr_s,funct3,ins)) {
	case FUNCT_SB: RV32I_SB(ctx,ins); break;
	case FUNCT_SH: RV32I_SH(ctx,ins); break;
	case FUNCT_SW: RV32I_SW(ctx,ins); break;
	default: goto illegal;
	}
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	RV32I_DECODE_SB;
	switch(bitfield_fetch(instr_sb,funct3,ins)) {
	case FUNCT_BEQ:
	    if (rv32i_beq(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	case FUNCT_BNE:
	    if (rv32i_bne(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	case FUNCT_BLT: 
	    if (rv32i_blt(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	case FUNCT_BGE: 
	    if (rv32i_bge(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	case FUNCT_BLTU:
	    if (rv32i_bltu(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	case FUNCT_BGEU:
	    if (rv32i_bgeu(ctx,_rs1,_rs2)) return pc+_imm;
	    break;
	default: 
	    goto illegal;
	}
	break;
    }
    case OPCODE_LUI:  { // U-type
	RV32I_LUI(ctx,ins);
	break;
    }
    case OPCODE_AUIPC: { // U-type
	RV32I_AUIPC(ctx,ins);
	break;
    }

    case OPCODE_JALR: {
	RV32I_DECODE_I;	
	return rv32i_jalr(ctx, _rd, _rs1, _imm);
    }
	
    case OPCODE_JAL: {   // Uj-type
	RV32I_DECODE_UJ;
	return pc+rv32i_jal(ctx,_rd,_imm);
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
