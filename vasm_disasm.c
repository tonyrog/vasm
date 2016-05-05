#include <stdio.h>
#include "vasm.h"

#define TAB "  "

char* register_name[32] = {
    "x0","x1","x2","x3","x4","x5","x6","x7","x8","x9",
    "x10","x11","x12","x13","x14","x15","x16","x17","x18","x19",
    "x20","x21","x22","x23","x24","x25","x26","x27","x28","x29",
    "x30","x31"
};

char* op_arith_name_00[] =  // funct7 == 0x00
{
    "add", "sll", "slt", "sltu", "xor", "srl", "or", "and"
};

char* op_arith_name_20[] =  // funct7 = 0x20
{
    "sub", "sll", "slt", "sltu", "xor", "sra", "or", "and"
};

char* op_imm_name_00[] = // imm11_0 = 0x00
{
    "addi", "slli", "slti", "sltiu", "xori", "srli", "ori", "andi"
};

char* op_imm_name_20[] =  // imm11_0 = 0x20
{
    "addi", "slli", "slti", "sltiu", "xori", "srai", "ori", "andi"
};

char* op_load_name[] =
{
    "lb", "lh", "lw", "???", "lbu", "lhu", "???", "???"
};

char* op_fence_name[] =
{
    "fence", "fence.i", "???", "???", "???", "???", "???", "???"
};

char* op_store_name[] =
{
    "sb", "sh", "sw", "???", "???", "???", "???", "???"
};

char* op_branch_name[] =
{
    "beq", "bne", "blt", "bltu", "bge", "bgeu", "???", "???"
};


void disasm_instr(FILE* f,symbol_table_t* symtab,unsigned addr,void* mem)
{
    void* p = (void*) ((uint8_t*)mem+addr);
    switch(((instr_t*)p)->opcode) {
    case OPCODE_ARITH: {    // R-type
	instr_r* ip = (instr_r*) p;
	switch(ip->funct7) {
	case 0x00: fprintf(f, "%s", op_arith_name_00[ip->funct3]); break;
	case 0x20: fprintf(f, "%s", op_arith_name_20[ip->funct3]); break;
	default: fprintf(f, "%s", "???"); break;
	}
	fprintf(f, " %s, %s, %s\n", 
		register_name[ip->rd], 
		register_name[ip->rs1],
		register_name[ip->rs2]);
	break;
    }
    case OPCODE_IMM: {  // I-type
	instr_i* ip = (instr_i*) p;
	switch(ip->imm11_0) {
	case 0x00: fprintf(f, "%s", op_imm_name_00[ip->funct3]); break;
	case 0x20: fprintf(f, "%s", op_imm_name_20[ip->funct3]); break;
	default: fprintf(f, "%s", "???"); break;
	}
	fprintf(f, " %s, %s, %d\n",
		register_name[ip->rd], 
		register_name[ip->rs1],
		ip->imm11_0);
	break;
    }

    case OPCODE_LOAD: {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", op_load_name[ip->funct3]);
	fprintf(f, " %s, %s, %d\n",
		register_name[ip->rd], 
		register_name[ip->rs1],
		ip->imm11_0);
	break;
    }
    case OPCODE_FENCE:  {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", op_fence_name[ip->funct3]);
	fprintf(f, " %s, %s, %d\n",
		register_name[ip->rd],
		register_name[ip->rs1],
		ip->imm11_0);
	break;
    }
    case OPCODE_JALR:  {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", "jalr");
	fprintf(f, " %s, %s, %d\n",
		register_name[ip->rd],
		register_name[ip->rs1],
		ip->imm11_0);
	break;
    }

    case OPCODE_SYS:  {  // I-type
	instr_i* ip = (instr_i*) p;
	if (ip->imm11_0 == 0)
	    fprintf(f, "%s", "ecall");
	else if (ip->imm11_0 == 1)
	    fprintf(f, "%s", "ebreak");
	else
	    fprintf(f, "%s", "???");
	fprintf(f, " %s, %s\n",
		register_name[ip->rd],
		register_name[ip->rs1]);
	break;
    }

    case OPCODE_STORE:  { // S-type
	instr_s* ip = (instr_s*) p;
	fprintf(f, "%s", op_store_name[ip->funct3]);
	fprintf(f, " %s, %d(%s)\n",
		register_name[ip->rs1], 
		(ip->imm11_5 << 5) + (ip->imm4_0),
		register_name[ip->rs2]);
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	instr_sb* ip = (instr_sb*) p;
	fprintf(f, "%s", op_branch_name[ip->funct3]);
	// fixme: sign
	fprintf(f, " %s, %s, %d\n",
		register_name[ip->rs1],
		register_name[ip->rs2],
		((ip->imm12<<12) | (ip->imm11<<11) |
		 (ip->imm10_5<<5) | (ip->imm4_1 << 1)));
	break;
    }
    case OPCODE_LUI: {  // U-type
	instr_u* ip = (instr_u*) p;
	fprintf(f, "%s", "lui");
	// fixme: sign
	fprintf(f, " %s, %d\n",
		register_name[ip->rd],
		ip->imm31_12);
	break;
    }

    case OPCODE_AUIPC: { // U-type
	instr_u* ip = (instr_u*) p;
	fprintf(f, "%s", "auipc");
	// fixme: sign
	fprintf(f, " %s, %d\n",
		register_name[ip->rd],
		ip->imm31_12);
	break;
    }
    case OPCODE_JAL: {  // Uj-type
	instr_uj* ip = (instr_uj*) p;
	fprintf(f, "%s", "jal");
	// fixme: sign
	fprintf(f, " %s, %d\n",
		register_name[ip->rd],
		(ip->imm20 << 20) |
		(ip->imm19_12 << 12) |
		(ip->imm11 << 11) |
		(ip->imm10_1 << 1));
	break;
    }
    default:
	break;
    }
}

void disasm(FILE* f, symbol_table_t* symtab,  void* mem, size_t n)
{
    unsigned_t addr;

    for (addr = 0; addr < n; addr++) {
	symbol_t* sym;
	if ((sym = symbol_table_find_label(symtab, addr)) != NULL) 
	    fprintf(f, "%s:\n", sym->name);
	disasm_instr(f, symtab, addr, mem);
    }
}
