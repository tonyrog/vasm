
#include "vasm_ccode.h"

#define TAB "    "


unsigned_t gen_ccode_instr(FILE* f,symbol_table_t* symtab,
			   unsigned_t addr,void* mem)
{
    return addr+4;
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
