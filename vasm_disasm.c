#include <stdio.h>
#include "vasm.h"

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
    int32_t  imm=0;
    int      rd=0, rs1=0, rs2=0;
    int      j;
    uint32_t seq;
    symbol_t* sym;
    unsigned_t addr1;


#if BYTE_ORDER == LITTLE_ENDIAN
    uint32_t ins = ((uint16_t*)p)[0];
#else
    uint32_t ins = ((uint8_t*)p)[0] | ((((uint8_t*)p)[1]) << 8);
#endif

#if defined(RV32C)
    if (OPCODE_IS_16BIT(ins)) {
	addr1 = addr+2;
	goto decode;
    }
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
    ins |= (((uint16_t*)p)[1] << 16);
#else
    ins |= (((uint8_t*)p)[2] | ((((uint8_t*)p)[3]) << 8)) << 16;
#endif
    addr1 = addr+4;

decode:
    if ((sym = symbol_lookup(symtab, ins)) == NULL) {
	printf("%08x: %08x ???\n", addr, ins);
	return addr+4;
    }

    fprintf(f, "%08x: ", addr);
    fprintf(f, "%08x m=%08x c=%08x ", ins, sym->mask, sym->code);
    fprintf(f, "%s ", sym->name);


    // extract arguments
    switch(sym->format) {
    case FORMAT_R:
	rd = bitfield_fetch(instr_r, rd, ins);
	rs1 = bitfield_fetch(instr_r, rs1, ins);
	rs2 = bitfield_fetch(instr_r, rs2, ins);	
	break;
    case FORMAT_I:
	rd = bitfield_fetch(instr_i, rd, ins);
	rs1 = bitfield_fetch(instr_i, rs1, ins);
	imm = bitfield_fetch(instr_i, imm11_0, ins);
	break;
    case FORMAT_S:
	rs1 = bitfield_fetch(instr_s, rs1, ins);
	rs2 = bitfield_fetch(instr_s, rs2, ins);
	imm = get_imm_s(ins);
	break;
    case FORMAT_SB:
	imm = get_imm_sb(ins);
	rs1 = bitfield_fetch(instr_sb, rs1, ins);
	rs2 = bitfield_fetch(instr_sb, rs2, ins);
	break;
    case FORMAT_U:
	rd = bitfield_fetch(instr_u, rd, ins);
	imm = bitfield_fetch(instr_u, imm31_12, ins);
	break;
    case FORMAT_UJ:
	rd = bitfield_fetch(instr_uj, rd, ins);
	imm = get_imm_uj(ins);
	break;
    case FORMAT_CR:
	rs2 = bitfield_fetch(instr_cr, rs2, ins);
	rd = bitfield_fetch(instr_cr, rd, ins);	
	break;
    case FORMAT_CI:
	rd = bitfield_fetch(instr_ci, rd, ins);
	imm = bitfield_fetch(instr_ci, imm6_2, ins) |
	    (bitfield_fetch(instr_ci, imm12, ins) << 5);
	break;
    case FORMAT_CSS:
    case FORMAT_CIW:
    case FORMAT_CL:
	break;
    case FORMAT_CS:
	rs2 = bitfield_fetch(instr_cs, rs2, ins);
	rd = bitfield_fetch(instr_cs, rd, ins);
	imm = bitfield_fetch(instr_cs, imm12_10, ins) << 2;
	break;
    case FORMAT_CB:
    case FORMAT_CJ:
	break;
    default:
	break;
    }

    seq = sym->sequence;
    j = 0;

#define NEXT_ARG if (j) { fprintf(f, ","); } else { j++; }
    // format arguments according to spec
    while(seq) {
	switch(seq & 0x1f) {
	case ASM_COPY_RD_RS1:  break;
	case ASM_CONST_0: break;
	case ASM_CONST_1: break;
	case ASM_CONST_MINUS_1: break;
	case ASM_RD_X0: break;
	case ASM_RD_X1: break;
	case ASM_REG_RD:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rd));
	    break;
	case ASM_REG_CRD:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rd+8));
	    break;
	case ASM_REG_RS1:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rs1));
	    break;
	case ASM_REG_CRS1:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rs1+8));
	    break;
	case ASM_REG_RS2:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rs2));
	    break;
	case ASM_REG_CRS2:
	    NEXT_ARG;
	    fprintf(f, "%s", register_abi_name(rs2+8));
	    break;
	case ASM_SHAMT_5:
	    NEXT_ARG;
	    fprintf(f, "%d", imm & 0x1f);
	    break;
	case ASM_IMM_6:
	case ASM_IMM_8:
	case ASM_IMM_12:
	    NEXT_ARG;
	    fprintf(f, "%d", imm);
	    break;
	case ASM_UIMM_20:  // load upper 20 bits
	    NEXT_ARG;
	    fprintf(f, "%d", imm << 12);
	    break;
	case ASM_REL_12:
	case ASM_REL_20:
	    NEXT_ARG;
	    fprintf(f, "%d", imm);
	    break;
	case ASM_IMM_12_RS1:  // optional imm12 (or 0) and rs1
	    NEXT_ARG;
	    if (imm == 0)
		fprintf(f, "(%s)", register_abi_name(rs1));
	    else
		fprintf(f, "%d(%s)", imm, register_abi_name(rs1));
	    break;
	case ASM_IMM_IORW:  // only works for two iorw arguments!
	    NEXT_ARG;
	    fprintf(f, "%s", iorw_name[(imm>>4)&0xf]);
	    imm <<= 4;
	    break;
	default:
	    fprintf(f, "?");
	    break;
	}
	seq >>= 5;
    }
    fprintf(f, "\n");

    return addr1;
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
