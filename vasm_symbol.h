#ifndef __VASM_SYMBOL_H__
#define __VASM_SYMBOL_H__

#include <stdio.h>
#include "vasm_types.h"
#include "vasm_rt.h"

#define SYMBOL_FLAG_IN_TABLE  0x01
#define SYMBOL_FLAG_LABEL     0x02
#define SYMBOL_FLAG_FORWARD   0x04
#define SYMBOL_FLAG_ON_HEAP   0x08  // entry is allocated on heap
#define SYMBOL_FLAG_REG       0x10  // symbol is a register name
#define SYMBOL_FLAG_INSTR     0x20  // symbol is an instruction name

typedef struct _symbol_link_t {
    struct _symbol_link_t* next;
    unsigned_t addr;
} symbol_link_t;

typedef struct _symbol_t {
    struct _symbol_t* next;
    unsigned_t flags;
    int        index;     // used for pre-installed symbols
    uint8_t    opcode;    // Instruction opcode
    uint16_t   funct;     // Instruction variant
    uint8_t    format;    // Instruction/Data format
    uint32_t   sequence;  // Parse sequence
    union {
	unsigned_t value;
	void* ptr;
	symbol_link_t* forward;
    };
    char* name;
    char  data[];   // normally name data
} symbol_t;

typedef struct {
    unsigned_t nsymbols;
    symbol_t* first;
    symbol_t* last;
} symbol_table_t;

#define SYM_INSTR(nm) \
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = #nm }

#define SYM_INSTR_X(nm,op,fu,fo,val,seq)					\
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, \
			  .opcode=(op), .funct=(fu), .format=(fo), .value=(val), .sequence=(seq), .name = #nm }

#define SYM_INSTR_NAME_X(nm,nam,op,fu,fo,val,seq)			\
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, \
			  .opcode=(op), .funct=(fu), .format=(fo), .value=(val), .sequence=(seq), .name = nam }

// when string name is not = nm
#define SYM_INSTR_NAME(nm,nam)						\
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = nam }

extern int symbol_eq(symbol_t* sym, char* name);
extern symbol_t* symbol_new(char* name,unsigned_t value);
extern void symbol_free(symbol_t* sym);

extern symbol_link_t* symbol_link_add(symbol_t* sym, unsigned addr);
extern void symbol_link_free(symbol_link_t* link);

extern void symbol_table_init(symbol_table_t* symtab);
extern void symbol_table_install(symbol_table_t* symtab, symbol_t* first);
extern void symbol_table_install_array(symbol_table_t* symtab, symbol_t* first,
				       size_t n);
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
