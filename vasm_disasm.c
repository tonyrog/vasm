#include <stdio.h>
#include "vasm.h"

#define TAB "  "

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
    "beq", "bne", "???", "???", "blt", "bge", "bltu", "bgeu"
};


char* iorw_name[] = 
{
           // IORW
    "",    // 0000
    "w",   // 0001
    "r",   // 0010
    "rw",  // 0011
    "o",   // 0100
    "ow",  // 0101
    "or",  // 0110
    "orw", // 0111
    "i",   // 1000
    "iw",   // 1001
    "ir",   // 1010
    "irw",  // 1011
    "io",   // 1100
    "iow",  // 1101
    "ior",  // 1110
    "iorw" // 1111
};

char* iorw(int v)
{
    return iorw_name[v&0xf];
}

unsigned_t disasm_instr(FILE* f,symbol_table_t* symtab,
			unsigned_t addr,void* mem)
{
    void* p = (void*) ((uint8_t*)mem+addr);
    switch(((instr_t*)p)->opcode) {
    case OPCODE_ARITH: {    // R-type
	instr_r* ip = (instr_r*) p;
	fprintf(f, "%s", TAB);
	switch(ip->funct7) {
	case 0x00: fprintf(f, "%s", op_arith_name_00[ip->funct3]); break;
	case 0x20: fprintf(f, "%s", op_arith_name_20[ip->funct3]); break;
	default: fprintf(f, "%s", "???"); break;
	}
	fprintf(f, " %s, %s, %s\n", 
		register_abi_name(ip->rd), 
		register_abi_name(ip->rs1),
		register_abi_name(ip->rs2));
	break;
    }

    case OPCODE_IMM: {  // I-type
	instr_i* ip = (instr_i*) p;
	int imm;
	int tab = 0x00;
	fprintf(f, "%s", TAB);
	switch(ip->funct3) {
	case FUNCT_SLLI:
	    imm = ip->imm11_0>>4;
	    break;
	case FUNCT_SRLI:
	    imm = ip->imm11_0 & 0x1f;
	    if ((ip->imm11_0 >> 4) == 0x20)
		tab = 0x20;
	    break;
	default:
	    imm = ip->imm11_0;
	    break;
	}
	switch(tab) {
	case 0x00: fprintf(f, "%s", op_imm_name_00[ip->funct3]); break;
	case 0x20: fprintf(f, "%s", op_imm_name_20[ip->funct3]); break;
	default: fprintf(f, "%s imm11_0=%x", "???", ip->imm11_0); break;
	}
	fprintf(f, " %s, %s, %d\n",
		register_abi_name(ip->rd), 
		register_abi_name(ip->rs1),
		imm);
	break;
    }

    case OPCODE_LOAD: {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_load_name[ip->funct3]);
	fprintf(f, " %s, %d(%s)\n",
		register_abi_name(ip->rd),
		ip->imm11_0,
		register_abi_name(ip->rs1));
	break;
    }

    case OPCODE_FENCE:  {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_fence_name[ip->funct3]);
	if (ip->funct3 == FUNCT_FENCE) {
	    fprintf(f, " %s, %s\n", 
		    iorw(ip->imm11_0 >> 4),
		    iorw(ip->imm11_0));
	}
	else {	    
	    fprintf(f, "\n");
	}
	break;
    }

    case OPCODE_SYS:  {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", TAB);
	switch(ip->imm11_0 & 0xfff) {
	case 0x000:
	    fprintf(f, "%s", "scall");
	    fprintf(f, " \n");
	    break;
	case 0x001:
	    fprintf(f, "%s", "sbreak");
	    fprintf(f, " \n");
	    break;
	case 0xc00:
	    fprintf(f, "%s", "rdcycle");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	case 0xc80:
	    fprintf(f, "%s", "rdcycleh");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	case 0xc01:
	    fprintf(f, "%s", "rdtime");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	case 0xc81:
	    fprintf(f, "%s", "rdtimeh");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	case 0xc02:
	    fprintf(f, "%s", "rdinstret");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	case 0xc82:
	    fprintf(f, "%s", "rdinstreth");
	    fprintf(f, " %s\n", register_abi_name(ip->rd));
	    break;
	default:
	    fprintf(f, "%s  imm11_0=%x", "???", ip->imm11_0);
	    fprintf(f, " %s, %s\n", 
		    register_abi_name(ip->rd),
		    register_abi_name(ip->rs1));
	    break;
	}
	break;
    }

    case OPCODE_STORE:  { // S-type
	instr_s* ip = (instr_s*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_store_name[ip->funct3]);
	fprintf(f, " %s, %d(%s)\n",
		register_abi_name(ip->rs1), 
		get_imm_s(ip),
		register_abi_name(ip->rs2));
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	instr_sb* ip = (instr_sb*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_branch_name[ip->funct3]);
	// fixme: sign
	fprintf(f, " %s, %s, %d\n",
		register_abi_name(ip->rs1),
		register_abi_name(ip->rs2),
		get_imm_sb(ip));
	break;
    }
    case OPCODE_LUI: {  // U-type
	instr_u* ip = (instr_u*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "lui");
	// fixme: sign
	fprintf(f, " %s, %d\n",
		register_abi_name(ip->rd),
		ip->imm31_12);
	break;
    }

    case OPCODE_AUIPC: { // U-type
	instr_u* ip = (instr_u*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "auipc");
	// fixme: sign
	fprintf(f, " %s, %d\n",
		register_abi_name(ip->rd),
		ip->imm31_12);
	break;
    }

    case OPCODE_JALR:  {  // I-type
	instr_i* ip = (instr_i*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "jalr");
	fprintf(f, " %s, %d(%s)\n",
		register_abi_name(ip->rd),
		ip->imm11_0,
		register_abi_name(ip->rs1));
	break;
    }

    case OPCODE_JAL: {  // Uj-type
	instr_uj* ip = (instr_uj*) p;
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "jal");
	fprintf(f, " %s, %d\n",
		register_abi_name(ip->rd),
		get_imm_uj(ip));
	break;
    }
    default:
	break;
    }
    return addr+4;
}

void disasm(FILE* f, symbol_table_t* symtab,  void* mem, size_t max_addr)
{
    unsigned_t addr = 0;

    while(addr < max_addr) {
	symbol_t* sym;
	if ((sym = symbol_table_find_label(symtab, addr)) != NULL)
	    fprintf(f, "%s:\n", sym->name);
	addr = disasm_instr(f, symtab, addr, mem);
    }
}
