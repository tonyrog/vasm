#include <stdio.h>
#include "vasm.h"

#define TAB "  "

char* op_arith_name_00[] =
{
    "add", "sll", "slt", "sltu", "xor", "srl", "or", "and"
};

char* op_arith_name_01[] =
{
    "mul", "mulh", "mulhsu", "mulhu", "div", "divu", "rem", "remu"
};

char* op_arith_name_20[] =
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
#if BYTE_ORDER == LITTLE_ENDIAN    
    uint32_t ins = ((uint16_t*)p)[0];
#else
    uint32_t ins = ((uint8_t*)p)[0] | ((((uint8_t*)p)[1]) << 8);
#endif

#if defined(RV32C)
    if (OPCODE_IS_16BIT(ins)) {
	switch(bitfield_fetch(instr_c,opcode,ins)) {
	case OPCODE_C0:
	    switch(bitfield_fetch(instr_c,funct3,ins)) {
	    case FUNCT_C_ADDI4SPN:
	    case FUNCT_C_FLD:
	    case FUNCT_C_FLW:
//	    case FUNCT_C_LD:
	    case FUNCT_C_FSD:
		// case FUNCT_C_SQ:
	    case FUNCT_C_SW:
	    case FUNCT_C_FSW:
		// case FUNCT_C_SD:
		break;
	    }
	    break;
	case OPCODE_C1:
	    switch(bitfield_fetch(instr_c,funct3,ins)) {
	    case FUNCT_C_NOP:
//	    case FUNCT_C_ADDI:
	    case FUNCT_C_ADDIW:
	    case FUNCT_C_LI:
//	    case FUNCT_C_ADDI16SP:
	    case FUNCT_C_LUI:
	    case FUNCT_C_SRLI:
//	    case FUNCT_C_SRAI:
//	    case FUNCT_C_ANDI:
//	    case FUNCT_C_SUB:
//	    case FUNCT_C_XOR:
//	    case FUNCT_C_OR:
//	    case FUNCT_C_AND:
//	    case FUNCT_C_SUBW:
//	    case FUNCT_C_ADDW:
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
//	    case FUNCT_C_MV:
//	    case FUNCT_C_EBREAK:
//	    case FUNCT_C_JALR:
//	    case FUNCT_C_ADD:
	    case FUNCT_C_FSDSP:
	    case FUNCT_C_SWSP:
	    case FUNCT_C_SDSP:
		break;
	    }
	    break;
	}
    }
    return addr+2;
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
    ins |= (((uint16_t*)p)[1] << 16);
#else
    ins |= (((uint8_t*)p)[2] | ((((uint8_t*)p)[3]) << 8)) << 16;
#endif
    fprintf(f, "%08x: ", addr);

    switch(bitfield_fetch(instr_t,opcode,ins)) {
    case OPCODE_ARITH: {    // R-type
	int funct3 = bitfield_fetch(instr_r,funct3,ins);
	fprintf(f, "%s", TAB);
	switch(bitfield_fetch(instr_r,funct7,ins)) {
	case 0x00: fprintf(f, "%s", op_arith_name_00[funct3]); break;
	case 0x01: fprintf(f, "%s", op_arith_name_01[funct3]); break;
	case 0x20: fprintf(f, "%s", op_arith_name_20[funct3]); break;
	default: fprintf(f, "%s", "???"); break;
	}
	fprintf(f, " %s, %s, %s", 
		register_abi_name(bitfield_fetch(instr_r,rd,ins)), 
		register_abi_name(bitfield_fetch(instr_r,rs1,ins)),
		register_abi_name(bitfield_fetch(instr_r,rs2,ins)));
	break;
    }

    case OPCODE_IMM: {  // I-type
	int funct3 = bitfield_fetch(instr_i,funct3,ins);
	int imm;
	int tab = 0x00;
	fprintf(f, "%s", TAB);
	switch(funct3) {
	case FUNCT_SLLI:
	    imm = bitfield_fetch_signed(instr_i,imm11_0,ins)>>4;
	    break;
	case FUNCT_SRLI:
	    imm = bitfield_fetch_signed(instr_i,imm11_0,ins) & 0x1f;
	    if ((bitfield_fetch_signed(instr_i,imm11_0,ins)>>4) == 0x20)
		tab = 0x20;
	    break;
	default:
	    imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	    break;
	}
	switch(tab) {
	case 0x00: fprintf(f, "%s", op_imm_name_00[funct3]); break;
	case 0x20: fprintf(f, "%s", op_imm_name_20[funct3]); break;
	default: 
	    fprintf(f, "%s imm11_0=%x", "???",
		    bitfield_fetch_signed(instr_i,imm11_0,ins));
	    break;
	}
	fprintf(f, " %s, %s, %d",
		register_abi_name(bitfield_fetch(instr_i,rd,ins)), 
		register_abi_name(bitfield_fetch(instr_i,rs1,ins)),
		imm);
	break;
    }

    case OPCODE_LOAD: {  // I-type
	int funct3 = bitfield_fetch(instr_i,funct3,ins);

	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_load_name[funct3]);
	fprintf(f, " %s, %d(%s)",
		register_abi_name(bitfield_fetch(instr_i,rd,ins)),
		bitfield_fetch_signed(instr_i,imm11_0,ins),
		register_abi_name(bitfield_fetch(instr_i,rs1,ins)));
	break;
    }

    case OPCODE_FENCE:  {  // I-typ
	int funct3 = bitfield_fetch(instr_i,funct3,ins);
	int imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_fence_name[funct3]);
	if (funct3 == FUNCT_FENCE) {
	    fprintf(f, " %s, %s", 
		    iorw(imm >> 4),
		    iorw(imm));
	}
	break;
    }

    case OPCODE_SYS:  {  // I-type
	int imm = bitfield_fetch_signed(instr_i,imm11_0,ins);
	fprintf(f, "%s", TAB);
	switch(imm & 0xfff) {
	case 0x000:
	    fprintf(f, "%s", "scall");
	    break;
	case 0x001:
	    fprintf(f, "%s", "sbreak");
	    break;
	case 0xc00:
	    fprintf(f, "%s", "rdcycle");
	    fprintf(f, " %s", 
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	case 0xc80:
	    fprintf(f, "%s", "rdcycleh");
	    fprintf(f, " %s", 
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	case 0xc01:
	    fprintf(f, "%s", "rdtime");
	    fprintf(f, " %s",
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	case 0xc81:
	    fprintf(f, "%s", "rdtimeh");
	    fprintf(f, " %s",
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	case 0xc02:
	    fprintf(f, "%s", "rdinstret");
	    fprintf(f, " %s",
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	case 0xc82:
	    fprintf(f, "%s", "rdinstreth");
	    fprintf(f, " %s", 
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)));
	    break;
	default:
	    fprintf(f, "%s  imm11_0=%x", "???", imm);
	    fprintf(f, " %s, %s", 
		    register_abi_name(bitfield_fetch(instr_i,rd,ins)),
		    register_abi_name(bitfield_fetch(instr_i,rs1,ins)));
	    break;
	}
	break;
    }

    case OPCODE_STORE:  { // S-type
	int funct3 = bitfield_fetch(instr_s,funct3,ins);
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_store_name[funct3]);
	fprintf(f, " %s, %d(%s)",
		register_abi_name(bitfield_fetch(instr_s,rs1,ins)), 
		get_imm_s(ins),
		register_abi_name(bitfield_fetch(instr_s,rs2,ins)));
	break;
    }

    case OPCODE_BRANCH: { // SB-type
	int funct3 = bitfield_fetch(instr_sb,funct3,ins);
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", op_branch_name[funct3]);
	// fixme: sign
	fprintf(f, " %s, %s, %d",
		register_abi_name(bitfield_fetch(instr_sb,rs1,ins)),
		register_abi_name(bitfield_fetch(instr_sb,rs2,ins)),
		get_imm_sb(ins));
	break;
    }
    case OPCODE_LUI: {  // U-type
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "lui");
	// fixme: sign
	fprintf(f, " %s, %d",
		register_abi_name(bitfield_fetch(instr_u,rd,ins)),
		bitfield_fetch(instr_u,imm31_12,ins) << 12);
	break;
    }

    case OPCODE_AUIPC: { // U-type
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "auipc");
	// fixme: sign
	fprintf(f, " %s, %d",
		register_abi_name(bitfield_fetch(instr_u,rd,ins)),
		bitfield_fetch(instr_u,imm31_12,ins) << 12);
	break;
    }

    case OPCODE_JALR:  {  // I-type
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "jalr");
	fprintf(f, " %s, %d(%s)",
		register_abi_name(bitfield_fetch(instr_i,rd,ins)),
		bitfield_fetch_signed(instr_i,imm11_0,ins),
		register_abi_name(bitfield_fetch(instr_i,rs1,ins)));
	break;
    }

    case OPCODE_JAL: {  // Uj-type
	fprintf(f, "%s", TAB);
	fprintf(f, "%s", "jal");
	fprintf(f, " %s, %d",
		register_abi_name(bitfield_fetch(instr_uj,rd,ins)),
		get_imm_uj(ins));
	break;
    }
    default:
	break;
    }
    fprintf(f, " [%08x]\n", ins);
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
