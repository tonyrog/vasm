#ifndef __VASM_SYMBOL_H__
#define __VASM_SYMBOL_H__

#include <stdio.h>
#include "vasm_types.h"

#define SYMBOL_FLAG_IN_TABLE 0x01
#define SYMBOL_FLAG_LABEL    0x02
#define SYMBOL_FLAG_FORWARD  0x04

typedef struct _symbol_link_t {
    struct _symbol_link_t* next;
    unsigned_t addr;
} symbol_link_t;

typedef struct _symbol_t {
    struct _symbol_t* next;
    unsigned_t flags;
    union {
	unsigned_t value;
	void* ptr;
	symbol_link_t* forward;
    };
    char name[];
} symbol_t;

typedef struct {
    unsigned_t nsymbols;
    symbol_t* first;
    symbol_t* last;
} symbol_table_t;


extern int symbol_eq(symbol_t* sym, char* name);
extern symbol_t* symbol_new(char* name,unsigned_t value);
extern void symbol_free(symbol_t* sym);

extern symbol_link_t* symbol_link_add(symbol_t* sym, unsigned addr);
extern void symbol_link_free(symbol_link_t* link);

extern void symbol_table_init(symbol_table_t* symtab);
extern symbol_t* symbol_table_add(symbol_table_t* symtab,char* name,
				  unsigned_t value);
extern symbol_t* symbol_table_remove(symbol_table_t* symtab, char* name);

extern symbol_t* symbol_table_push(symbol_table_t* symtab,char* name,
				   unsigned_t value);
extern symbol_t* symbol_table_pop(symbol_table_t* symtab);
extern symbol_t* symbol_table_lookup(symbol_table_t* symtab, char* name);
extern symbol_t* symbol_table_find_label(symbol_table_t* symtab,
					 unsigned_t addr);
extern void symbol_table_dump(FILE* out, symbol_table_t* symtab);

#endif
