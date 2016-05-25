
#include "vasm_ccode.h"

#define TAB "    "


unsigned_t gen_ccode_instr(FILE* f,symbol_table_t* symtab,
			   unsigned_t addr,void* mem)
{
    return addr+4;
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
		    fprintf(f, "%sif ((ins & 0x%08x) == 0x%08x) { EMU_%s; }\n", 
			    tab(lev+1), sym->mask, sym->code, sym->name);
		}
	    }
	    fprintf(f, "%sbreak;\n", tab(lev+1));
	}
	fprintf(f, "%s}\n", tab(lev));
    }
    else {
	symbol_t* sym = np->sym;
	fprintf(f, "%sif ((ins & 0x%08x) == 0x%08x) { EMU_%s; }\n", 
		tab(lev), sym->mask, sym->code, sym->name);
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
