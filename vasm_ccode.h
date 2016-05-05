#ifndef __VASM_CCODE_H__
#define __VASM_CCODE_H__

#include <stdio.h>
#include "vasm.h"
#include "vasm_symbol.h"

extern void gen_ccode_operand(FILE* f, int mode, int reg);
extern unsigned_t gen_ccode_instr(FILE* f, symbol_table_t* symtab, 
				  unsigned_t addr, void* mem);
extern void gen_ccode(FILE* f, symbol_table_t* symtab, void* mem, size_t n);

#endif
