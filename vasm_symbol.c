#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vasm_symbol.h"

// fixme: hash when needd

int symbol_eq(symbol_t* sym, char* name)
{
    return strcmp(sym->name, name) == 0;
}

symbol_t* symbol_new(char* name,unsigned_t value)
{
    symbol_t* sym;
    size_t len = strlen(name);

    if ((sym = malloc(sizeof(symbol_t)+len+1)) == NULL)
	return NULL;
    sym->next = NULL;
    sym->value = value;
    sym->flags = SYMBOL_FLAG_ON_HEAP;
    memcpy(sym->data, name, len);
    sym->data[len] = '\0';
    sym->name = sym->data;
    // fprintf(stderr, "new symbol '%s' value = %u\n", sym->name, sym->value);
    return sym;
}

void symbol_free(symbol_t* sym)
{
    if (sym == NULL)
	return;
    if (sym->flags & SYMBOL_FLAG_ON_HEAP)
	return;
    if (sym->flags & SYMBOL_FLAG_IN_TABLE) {
	fprintf(stderr, "free symbol '%s' still in table!\n", sym->name);
	return;
    }
    free(sym);
}

symbol_t* symbol_table_add(symbol_table_t* symtab,char* name,unsigned_t value)
{
    symbol_t* sym;

    if ((sym = symbol_new(name,value)) == NULL)
	return NULL;
    if (symtab->last != NULL)
	symtab->last->next = sym;
    else
	symtab->first = sym;
    sym->flags |= (SYMBOL_FLAG_IN_TABLE);
    symtab->last = sym;
    symtab->nsymbols++;
    return sym;
}

// remove from symbol table but do not delete
symbol_t* symbol_table_remove(symbol_table_t* symtab, char* name)
{
    symbol_t* prev;
    symbol_t* sym;

    for (sym = symtab->first, prev=NULL; sym; prev = sym, sym = sym->next) {
	if (symbol_eq(sym, name) == 0) {
	    if (prev == NULL) {
		if ((symtab->first = prev) == NULL)
		    symtab->last = NULL;
	    }
	    else {
		if ((prev->next = sym->next) == NULL)
		    symtab->last = prev;
	    }
	    symtab->nsymbols--;
	    sym->flags &= ~SYMBOL_FLAG_IN_TABLE;
	    return sym;
	}
    }
    return NULL;
}

symbol_t* symbol_table_push(symbol_table_t* symtab,char* name,unsigned_t value)
{
    symbol_t* sym;
    
    if ((sym = symbol_new(name,value)) == NULL)
	return NULL;
    if ((sym->next = symtab->first) == NULL)
	symtab->last = sym;
    symtab->first = sym;
    symtab->nsymbols++;
    sym->flags |= SYMBOL_FLAG_IN_TABLE;
    return sym;
}

symbol_t* symbol_table_pop(symbol_table_t* symtab)
{
    symbol_t* sym = symtab->first;
    if ((symtab->first = sym->next) == NULL)
	symtab->last = NULL;
    symtab->nsymbols--;
    sym->flags &= ~SYMBOL_FLAG_IN_TABLE;
    return sym;
}

// install linked symbols into symbol table ( fixed or dynamic )
void symbol_table_install(symbol_table_t* symtab, symbol_t* first)
{
    if (first != NULL) {
	if (symtab->last != NULL)
	    symtab->last->next = first;
	else
	    symtab->first = first;
	symtab->nsymbols++;
	while(first->next) {
	    symtab->nsymbols++;
	    first = first->next;
	}
	symtab->last = first;
    }
}

// install linked symbols from array into symbol table
void symbol_table_install_array(symbol_table_t* symtab, symbol_t* first, size_t n)
{
    if ((first != NULL) && (n > 0)) {
	if (symtab->last != NULL)
	    symtab->last->next = first;
	else
	    symtab->first = first;
	symtab->last = first+(n-1);
	assert(symtab->last->next != NULL);
	symtab->nsymbols += n;
    }
}

symbol_t* symbol_table_lookup(symbol_table_t* symtab, char* name)
{
    symbol_t* sym;
    for (sym = symtab->first; sym; sym = sym->next) {
	if (symbol_eq(sym, name))
	    return sym;
    }
    return NULL;
}

symbol_t* symbol_table_find_label(symbol_table_t* symtab, unsigned_t addr)
{
    symbol_t* sym;
    for (sym = symtab->first; sym; sym = sym->next) {
	if ((sym->value == addr) && (sym->flags & SYMBOL_FLAG_LABEL))
	    return sym;
    }
    return NULL;
}

symbol_link_t* symbol_link_add(symbol_t* sym, unsigned addr)
{
    symbol_link_t* link;

    if ((link = malloc(sizeof(symbol_link_t))) == NULL)
	return NULL;
    link->next = sym->forward;
    link->addr = addr;
    sym->forward = link;
    return link;
}

void symbol_link_free(symbol_link_t* link)
{
    free(link);
}

void symbol_table_init(symbol_table_t* symtab)
{
    symtab->nsymbols = 0;
    symtab->first = NULL;
    symtab->last = NULL;
}

void symbol_table_dump(FILE* out, symbol_table_t* symtab)
{
    symbol_t* sym;
    for (sym = symtab->first; sym; sym = sym->next) {
	if (sym->flags & SYMBOL_FLAG_FORWARD) {
	    symbol_link_t* link = sym->forward;
	    fprintf(out, "%s = ", sym->name);
	    while(link) {
		fprintf(out, "%u,", link->addr);
		link = link->next;
	    }
	    fprintf(out, "0\n");
	}
	else {
	    fprintf(out, "%s = %u\n", sym->name, sym->value);
	}
    }
}
