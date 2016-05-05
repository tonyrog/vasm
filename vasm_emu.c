
#include "vasm.h"
#include "vasm_rt.h"

// when implementing compressed instructions are 16bit aligned
unsigned_t emu(vasm_rt_t* ctx,unsigned_t addr, void* mem)
{
    void* p = (void*) ((uint8_t*)mem+addr);
    switch(((instr_t*)p)->opcode) {
    case OPCODE_ARITH: {    // R-type
	instr_r* ip = (instr_r*) p;
	switch(ip->funct3) {
	case FUNCT_ADD:  // funct7 = 0000000
     // case FUNCT_SUB:  // funct7 = 0100000
	    if (ip->funct7 == 0x00)
		ctx->reg[ip->rd] = ctx->reg[ip->rs1] + ctx->reg[ip->rs2];
	    else if (ip->funct7 == 0x20)
		ctx->reg[ip->rd] = ctx->reg[ip->rs1] - ctx->reg[ip->rs2];
	    break;
	case FUNCT_SLL:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] << ctx->reg[ip->rs2];
	    break;
	case FUNCT_SLT:
	    if ((int32_t)ctx->reg[ip->rs1] < (int32_t)ctx->reg[ip->rs2])
		ctx->reg[ip->rd] = 1;
	    break;
	case FUNCT_SLTU:
	    if (ctx->reg[ip->rs1] < ctx->reg[ip->rs2])
		ctx->reg[ip->rd] = 1;
	    break;
	case FUNCT_XOR:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] ^ ctx->reg[ip->rs2];
	    break;
	case FUNCT_SRL:   // funct7 = 0000000
     // case FUNCT_SRA:   // funct7 = 0100000
	    if (ip->funct7 == 0x00)
		ctx->reg[ip->rd] = ctx->reg[ip->rs1] >> ctx->reg[ip->rs2];
	    else if (ip->funct7 == 0x20)
		ctx->reg[ip->rd] = (int32_t)ctx->reg[ip->rs1] >> ctx->reg[ip->rs2];
	    break;
	case FUNCT_OR:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] | ctx->reg[ip->rs2];
	    break;
	case FUNCT_AND:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] & ctx->reg[ip->rs2];
	    break;
	}
	break;
    }
    case OPCODE_IMM: {   // I-type
	instr_i* ip = (instr_i*) p;
	switch(ip->funct3) {
	case FUNCT_ADDI:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] + ip->imm11_0;
	    break;
	case FUNCT_SLLI:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] << ip->imm11_0;
	    break;
	case FUNCT_SLTI:
	    if ((int32_t)ctx->reg[ip->rs1] < ip->imm11_0)
		ctx->reg[ip->rd] = 1;
	    break;
	case FUNCT_SLTIU:
	    if (ctx->reg[ip->rs1] < (unsigned)ip->imm11_0)
		ctx->reg[ip->rd] = 1;
	    break;
	case FUNCT_XORI:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] ^ ip->imm11_0;
	    break;
	case FUNCT_SRLI:  // imm11_0 = 0000000
     // case FUNCT_SRAI:   // imm11_0 = 0100000
	    if (ip->imm11_0 == 0x00)
		ctx->reg[ip->rd] = ctx->reg[ip->rs1] >> ip->imm11_0;
	    else if (ip->imm11_0 == 0x20)
		ctx->reg[ip->rd] = (int32_t)ctx->reg[ip->rs1] >> ip->imm11_0;
	    break;
	case FUNCT_ORI:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] | ip->imm11_0;
	    break;
	case FUNCT_ANDI:
	    ctx->reg[ip->rd] = ctx->reg[ip->rs1] & ip->imm11_0;
	    break;
	}
	break;	
    }

    case OPCODE_LOAD:  // I-type
    case OPCODE_FENCE: // I-type
    case OPCODE_SYS:   // I-type
    case OPCODE_STORE:  // S-type
    case OPCODE_BRANCH: // SB-type
    case OPCODE_LUI:    // U-type
    case OPCODE_AUIPC:  // U-type
    case OPCODE_JAL:    // Uj-type
	break;
    }
    return addr+4;
}

void dump_regs(FILE* f, vasm_rt_t* ctx)
{
    int i;

    for (i = 0; i < NUM_REGISTERS; i++) {
	fprintf(f, "x%02d = %08x", i, ctx->reg[i]);
	if ((i & 3) == 3) 
	    fprintf(f, "\n");
	else
	    fprintf(f, "    ");
    }
}

void run(FILE* f, symbol_table_t* symtab, vasm_rt_t* ctx, unsigned_t addr)
{
    dump_regs(f, ctx);
    while(addr < ctx->mem_addr) {
	disasm_instr(f, symtab, addr, ctx->mem);
	addr = emu(ctx, addr, ctx->mem);
	dump_regs(f, ctx);
    }
}
