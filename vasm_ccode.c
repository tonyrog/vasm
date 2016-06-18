#include <ctype.h>

#include "vasm_ccode.h"

#define TAB "    "

char* upper_string(char* name, char* dst, size_t maxlen)
{
    char* ptr = dst;
    while(maxlen-- && *name) {
	int c = *name++;
	*ptr++ = toupper(c);
    }
    if (maxlen)
	*ptr = '\0';
    else if (ptr > dst)
	*(ptr-1) = '\0';
    return dst;
}

char* c_name(char* name, char* dst, size_t maxlen)
{
    char* ptr = dst;
    while(maxlen-- && *name) {
	int c = *name++;
	if (c == '.')
	    *ptr++ = '_';
	else
	    *ptr++ = c;
    }
    if (maxlen)
	*ptr = '\0';
    else if (ptr > dst)
	*(ptr-1) = '\0';
    return dst;
}

char* uc_name(char* name, char* dst, size_t maxlen)
{
    char* ptr = dst;
    while(maxlen-- && *name) {
	int c = *name++;
	if (c == '.')
	    *ptr++ = '_';
	else
	    *ptr++ = toupper(c);
    }
    if (maxlen)
	*ptr = '\0';
    else if (ptr > dst)
	*(ptr-1) = '\0';
    return dst;
}

//
// Generate assembler instructions as C code using
// instruction in vasm_emu.h
//
unsigned_t gen_ccode_instr(FILE* f,symbol_table_t* symtab,
			   unsigned_t addr,void* mem)
{
    void* p = (void*) ((uint8_t*)mem+addr);
    int32_t  imm=0;
    int      rd=0, rs1=0, rs2=0;
    symbol_t* sym;
    symbol_t* dst;
    unsigned_t addr1;
    char buf[64];

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

    // extract arguments
    switch(sym->format) {
    case FORMAT_R:
	rd = bitfield_fetch(instr_r, rd, ins);
	rs1 = bitfield_fetch(instr_r, rs1, ins);
	rs2 = bitfield_fetch(instr_r, rs2, ins);
	fprintf(f, "  rv%s_%s(ctx,%d,%d,%d);\n",
		sym->ext,c_name(sym->name,buf,sizeof(buf)),
		rd,rs1,rs2);
	break;
    case FORMAT_I:
	rd = bitfield_fetch(instr_i, rd, ins);
	rs1 = bitfield_fetch(instr_i, rs1, ins);
	imm = bitfield_fetch(instr_i, imm11_0, ins);
	fprintf(f, "  rv%s_%s(ctx,%d,%d,%d);\n",
		sym->ext,c_name(sym->name,buf,sizeof(buf)),rd,rs1,imm);
	break;
    case FORMAT_S:
	rs1 = bitfield_fetch(instr_s, rs1, ins);
	rs2 = bitfield_fetch(instr_s, rs2, ins);
	imm = get_imm_s(ins);
	fprintf(f, "  rv%s_%s(ctx,%d,%d,%d);\n",
		sym->ext,c_name(sym->name,buf,sizeof(buf)),rs1, rs2, imm);
	break;
    case FORMAT_SB:
	imm = get_imm_sb(ins);
	rs1 = bitfield_fetch(instr_sb, rs1, ins);
	rs2 = bitfield_fetch(instr_sb, rs2, ins);
	if ((dst = symbol_table_find_label(symtab, addr+imm)) != NULL)
	    fprintf(f, "  if (rv%s_%s(ctx,%d,%d)) goto %s;\n",
		    sym->ext,c_name(sym->name,buf,sizeof(buf)),rs1,rs2,dst->name);
	else {
	    // add addr+imm as label ???
	    fprintf(f, "  if (rv%s_%s(ctx,%d,%d)) goto L%d;\n",
		    sym->ext,c_name(sym->name,buf,sizeof(buf)),rs1,rs2,addr+imm);
	}
	break;
    case FORMAT_U:
	rd = bitfield_fetch(instr_u, rd, ins);
	imm = bitfield_fetch(instr_u, imm31_12, ins);
	fprintf(f, "  rv%s_%s(ctx,%d,%d);\n",
		sym->ext,c_name(sym->name,buf,sizeof(buf)),rd,imm);
	break;
    case FORMAT_UJ:
	rd = bitfield_fetch(instr_uj, rd, ins);
	imm = get_imm_uj(ins);
	// fixme: generate with return label if rd!
	if ((dst = symbol_table_find_label(symtab, addr+imm)) != NULL) {
	    if ((strcmp(sym->name, "jal") == 0) ||
		(strcmp(sym->name, "j") == 0)) {
		if (rd == 0)
		    fprintf(f, "  goto %s;\n", dst->name);
		else 
		    fprintf(f, "  vasm_call(ctx,%d,%s);\n", rd,dst->name);
	    }
	    else {
		fprintf(f, "  rv%s_%s(ctx,%d,%s);\n",
			sym->ext,c_name(sym->name,buf,sizeof(buf)),rd,dst->name);
	    }
	}
	else {
	    if ((strcmp(sym->name, "jal") == 0)||
		(strcmp(sym->name, "j") == 0)) {
		if (rd == 0)
		    fprintf(f, "  goto L%d;\n", addr+imm);
		else 
		    fprintf(f, "  vasm_call(ctx,%d,L%d);\n", rd,addr+imm);
	    }
	    else {
		fprintf(f, "  rv%s_%s(ctx,%d,L%d);\n",
			sym->ext,c_name(sym->name,buf,sizeof(buf)),rd,addr+imm);
	    }
	}
	break;
    default:
	break;
    }
    return addr1;
}

static char* tab(int lev)
{
    if (lev >= 10)
	lev = 9;
    return &"                    "[(18-2*lev)];
}


void gen_tree_emu(FILE* f, sym_node_t* np, int lev)
{
    if (np->nbuckets) {
	int i;
	fprintf(f, "%sswitch((ins>>%d) & 0x%x) {\n", 
		tab(lev), np->shr, np->mask);
	for (i = 0; i < np->nbuckets; i++) {
	    sym_node_t* npi;
	    fprintf(f, "%scase %d:\n", tab(lev), i);
	    if ((npi = np->bucket[i]) != NULL) {
		if (npi->flags & SYMBOL_FLAG_NODE)
		    gen_tree_emu(f, npi, lev+1);
		else {
		    symbol_t* sym = npi->sym;
		    char buf[64];
		    char buf2[64];
		    fprintf(f, "%sif ((ins & 0x%08x) == 0x%08x) { RV%s_%s(ctx,ins); }\n",
			    tab(lev+1), sym->mask, sym->code, 
			    upper_string(sym->ext,buf2,sizeof(buf2)),
			    uc_name(sym->name,buf,sizeof(buf)));
		}
	    }
	    fprintf(f, "%sbreak;\n", tab(lev+1));
	}
	fprintf(f, "%s}\n", tab(lev));
    }
    else {
	symbol_t* sym = np->sym;
	char buf[64];
	char buf2[64];
	fprintf(f, "%sif ((ins & 0x%08x) == 0x%08x) { RV%s_%s(ctx,ins); }\n", 
		tab(lev), sym->mask, sym->code, 
		upper_string(sym->ext,buf2,sizeof(buf2)),
		uc_name(sym->name,buf,sizeof(buf)));
    }
}


// generate emulator from symbol table list or tree
void gen_emu(FILE* f, symbol_table_t* symtab)
{
    if (symtab->root)
	gen_tree_emu(f, symtab->root, 0);
}


void gen_ccode(FILE* f, symbol_table_t* symtab, void* mem, size_t n)
{
    unsigned_t addr = 0;

    fprintf(f, "#include \"vasm_rt.h\"\n\n");
    fprintf(f, 
	    "void vasm_code_segment(vasm_rt_t* ctx, unsigned_t entry)\n"
	    "{\n");
    while(addr < n) {
	symbol_t* sym;
	if ((sym = symbol_table_find_label(symtab, addr)) != NULL) 
	    fprintf(f, "%s:\n", sym->name);
	addr = gen_ccode_instr(f, symtab, addr, mem);
    }
    fprintf(f, "}\n\n");

    fprintf(f, "int main(int argc, char** argv)\n"
	    "{\n"
	    "  vasm_rt_t ctx;\n"
	    "  memset(&ctx, 0, sizeof(ctx));\n"
	    "  vasm_code_segment(&ctx, 0);\n"
	    "  exit(0);\n"
	    "}\n");
}
