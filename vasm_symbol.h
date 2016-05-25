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
#define SYMBOL_FLAG_NODE      0x80  // entry is a node

// index = (value >> shl) & mask
// node = bucket[index]
typedef struct _sym_node_t {
    uint32_t flags;             // SYMBOL_NODE else SYMBOL
    struct _sym_node_t* next;   // next sym_node in a list
    uint32_t shr;               // shift right bits
    uint32_t mask;              // then mask this
    size_t   nbuckets;          // to get an index less than nbuckets
    union {
	struct _sym_node_t** bucket;  // flags&SYMBOL_NODE
	struct _symbol_t*    sym;     // !flags&SYMBOL_NODE
    };
} sym_node_t;

// forward list of addresses
typedef struct _symbol_link_t {
    struct _symbol_link_t* next;
    unsigned_t addr;
} symbol_link_t;

typedef struct _symbol_t {
    uint32_t flags;
    struct _symbol_t* next;
    int        index;     // used for pre-installed symbols
    uint32_t   code;      // bit code
    uint32_t   mask;      // bit mask
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
    symbol_t** lastp;
    sym_node_t* root;
} symbol_table_t;


#define SYM_INSTR(nm) \
    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = #nm }

#define SYM_INSTR_X(nm,cod,fo,ma,seq) \
    [INSTR_##nm##_SI] = { .next = 0,\
			  .flags = SYMBOL_FLAG_INSTR,\
			  .index = INSTR_##nm##_SI,\
			  .format=(fo),\
			  .code=(cod), \
			  .mask=(ma),\
			  .sequence=(seq),\
			  .name = #nm }

#define SYM_INSTR_NAME_X(nm,nam,cod,fo,ma,seq)				\
    [INSTR_##nm##_SI] = { .next = 0,\
			  .flags = SYMBOL_FLAG_INSTR,\
			  .index = INSTR_##nm##_SI,\
			  .format=(fo),\
			  .code=(cod), \
			  .mask=(ma),\
			  .sequence=(seq),\
			  .name = nam }

// when string name is not = nm
// #define SYM_INSTR_NAME(nm,nam)					\
//    [INSTR_##nm##_SI] = { .next = 0, .flags = SYMBOL_FLAG_INSTR, .index = INSTR_##nm##_SI, .name = nam }

extern int symbol_eq(symbol_t* sym, char* name);
extern symbol_t* symbol_new(char* name,unsigned_t value);
extern void symbol_free(symbol_t* sym);

extern symbol_link_t* symbol_link_add(symbol_t* sym, unsigned addr);
extern void symbol_link_free(symbol_link_t* link);

extern void symbol_table_init(symbol_table_t* symtab);
extern int  symbol_tree_init(symbol_table_t* symtab);
extern void symbol_table_sort(symbol_table_t* symtab);
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
symbol_t* symbol_tree_lookup(sym_node_t* np, uint32_t ins);
symbol_t* symbol_lookup(symbol_table_t* np, uint32_t ins);
extern void symbol_table_dump(FILE* out, symbol_table_t* symtab);

#endif
