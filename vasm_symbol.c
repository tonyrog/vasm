#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vasm.h"
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
    *symtab->lastp = sym;
    symtab->lastp = &sym->next;
    sym->flags |= (SYMBOL_FLAG_IN_TABLE);
    symtab->nsymbols++;
    return sym;
}

// remove from symbol table but do not delete
symbol_t* symbol_table_remove(symbol_table_t* symtab, char* name)
{
    symbol_t** ss0 = &symtab->first;

    while(*ss0 && !symbol_eq(*ss0, name))
	ss0 = &(*ss0)->next;
    if (*ss0) {
	symbol_t* s0 = *ss0;
	if ((*ss0 = s0->next) == NULL)
	    symtab->lastp = ss0;
	symtab->nsymbols--;
	s0->flags &= ~SYMBOL_FLAG_IN_TABLE;
	return s0;
    }
    return NULL;
}

symbol_t* symbol_table_push(symbol_table_t* symtab,char* name,unsigned_t value)
{
    symbol_t* s0;
    
    if ((s0 = symbol_new(name,value)) == NULL)
	return NULL;
    if ((s0->next = symtab->first) == NULL)
	symtab->lastp = &s0->next;
    symtab->first = s0;
    symtab->nsymbols++;
    s0->flags |= SYMBOL_FLAG_IN_TABLE;
    return s0;
}

symbol_t* symbol_table_pop(symbol_table_t* symtab)
{
    symbol_t* s0 = symtab->first;
    if ((symtab->first = s0->next) == NULL)
	symtab->lastp = &symtab->first;
    symtab->nsymbols--;
    s0->flags &= ~SYMBOL_FLAG_IN_TABLE;
    return s0;
}

// install linked symbols into symbol table ( fixed or dynamic )
void symbol_table_install(symbol_table_t* symtab, symbol_t* s0)
{
    if (s0 != NULL) {
	*symtab->lastp = s0;
	symtab->nsymbols++;
	while(s0->next) {
	    symtab->nsymbols++;
	    s0 = s0->next;
	}
	symtab->lastp = &s0->next;
    }
}

// install linked symbols from array into symbol table
void symbol_table_install_array(symbol_table_t* symtab, symbol_t* s0, size_t n)
{
    if ((s0 != NULL) && (n > 0)) {
	int i;
	*symtab->lastp = s0;
	for (i = 0; i < n; i++)
	    s0[i].next = &s0[i+1];
	s0[n-1].next = 0;
	symtab->lastp = &s0[n-1].next;
	assert(*symtab->lastp == NULL);
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
    symtab->lastp = &symtab->first;
    symtab->root = NULL;
}

//
// sort symbols (instructions) so that the symbols with 
// more mask bits are before symbols with fewer mask bits
// list bubble-sort
//
static int is_bigger(symbol_t* a, symbol_t* b)
{
    int az = OPCODE_IS_16BIT(a->code) ? 16 : 32;
    int bz = OPCODE_IS_16BIT(b->code) ? 16 : 32;
    int an;
    int bn;
    if (az > bz) return 1;
    if (az < bz) return 0;
    an = __builtin_popcount(a->mask);
    bn = __builtin_popcount(b->mask);
    if (an > bn) return 1;
    if (an < bn) return 0;
    if (a->code > b->code)
	return 1;
    return 0;
}

void symbol_table_sort(symbol_table_t* symtab)
{
    symbol_t** spp1 = &symtab->first;

    while(*spp1) {
	while(*spp1 && !((*spp1)->flags & SYMBOL_FLAG_INSTR))
	    spp1 = &(*spp1)->next;

	if (*spp1) {
	    symbol_t** spp2 = &(*spp1)->next;
	    symbol_t** spp3 = spp1;
	    while(*spp2) {
		if ((*spp2)->flags & SYMBOL_FLAG_INSTR) {
		    if (is_bigger(*spp2, *spp3))
			spp3 = spp2;
		    spp2 = &(*spp2)->next;
		}
	    }
	    if (spp3 != spp1) {  // found a bigger element
		// unlink
		symbol_t* sp3 = *spp3;
//	    printf("swap %s (%d) with %s (%d)\n", 
//		   (*spp1)->name, __builtin_popcount((*spp1)->mask),
//		   (*spp3)->name, __builtin_popcount((*spp3)->mask));
		if ((*spp3 = sp3->next) == NULL)
		    symtab->lastp = spp3;
		// move to spp1
		sp3->next = *spp1;
		*spp1 = sp3;
	    }
	    spp1 = &(*spp1)->next;
	}
    }
}


static sym_node_t* new_sym_node(void)
{
    sym_node_t* ptr = malloc(sizeof(sym_node_t));
    if (ptr)
	memset(ptr, 0, sizeof(sym_node_t));
    ptr->flags |= SYMBOL_FLAG_NODE;
    return ptr;
}

// 1010100111 = 3
int number_of_consecutive_bits(uint32_t m)
{
    int i = 0;
    while(m && (m & 1)) {
	i++;
	m >>= 1;
    }
    return i;
}

// 1010100000 = 5
int number_of_zconsecutive_bits(uint32_t m)
{
    int i = 0;
    while(m && !(m & 1)) {
	i++;
	m >>= 1;
    }
    return i;
}

#if 0
static char* tab(int lev)
{
    if (lev > 10)
	lev = 10;
    return &"\t\t\t\t\t\t\t\t\t\t\t"[(10-lev)];
}
#endif

static sym_node_t* new_sym_node_array(sym_node_t** root, sym_node_t* list, int shr, int lev)
{
    uint32_t mask = 0xffffffff;
    uint32_t umask = 0;
    sym_node_t* rp;
    sym_node_t* np;
    int i, r, s=0;
    int len=0;
    int nfill;

    // printf("%sshr=%d\n", tab(lev), shr);

#ifdef DEBUG
    i=0;
    for (np = list; np; np = np->next) {
	printf("%s%d:%s %x\n", tab(lev), i,np->sym->name,(np->sym->mask>>shr));
	i++;
    }
#endif
    
    // calculate the mask
    for (np = list; np; np = np->next) {
	mask &= (np->sym->mask >> shr);
	umask |= (np->sym->mask >> shr);
	len++;
    }
    // printf("%smask=%x, umask=%x\n", tab(lev), mask, umask);

    if (len == 1) {  // must code for checking rest of bits!
	*root = list;
	return list;
    }

    if (!(mask & 1)) {  // remove leading zero from mask
	s = number_of_zconsecutive_bits(mask);
	mask >>= s;
	shr += s;
    }
    // printf("%smask=%x, shr=%d\n", tab(lev), mask, shr);

    if (mask == 0) {
//	for (np = list; np; np = np->next)
//	    printf("%snon-unique mask=0 sym=%s\n", tab(lev), np->sym->name);
	*root = list;
	return list;
    }

    r = number_of_consecutive_bits(mask);
    // printf("%smask = 0x%x, s=%d, r=%d\n", tab(lev), mask, s, r);

    rp = new_sym_node();  // create parent node
    rp->shr = shr;        // total bits to shift to next level

    // allocate mask+1 slots
    // fixme: shift mask until bit 0 is = 1 only use consecutive bits
    if ((r <= 8) && ((1 << r)/len <= 2)) {
	rp->nbuckets = (1<<r);
	if ((rp->bucket = (sym_node_t**) malloc(sizeof(sym_node_t*)*rp->nbuckets)) != NULL)
	    memset(rp->bucket, 0, sizeof(sym_node_t*)*rp->nbuckets);
	else
	    return NULL;
    }
    else {
	// to many bits to be able to do direct jump
	// fixme: split in fixed steps
	*root = list;
	return list;
    }
    
    // split list into buckets
    np = list;
    mask = (1<<r)-1;
    rp->mask = mask;
    nfill = 0;    // number of filled buckets
    while(np) {
	sym_node_t* nnp = np->next;
	int index = rp->mask & (np->sym->code >> shr);
	// printf("%s%s => %d\n", tab(lev), np->sym->name, index);
	if (rp->bucket[index] == NULL) nfill++;
	np->next = rp->bucket[index];
	rp->bucket[index] = np;
	np = nnp;
    }

    for (i = 0; i < rp->nbuckets; i++) {
	int count = 0;
	for (np = rp->bucket[i]; np != NULL; np = np->next)
	    count++;
	// printf("%sbucket[%d] = %d\n", tab(lev), i, count);
	if (count == len) {
	    *root = rp->bucket[i];
	    free(rp);
	    return *root;
	}
	if (count > 1) {
	    new_sym_node_array(&rp->bucket[i], rp->bucket[i], shr+r, lev+1);
	}
    }
    *root = rp;
    return rp;
}

//
// install all sym in a tree
//

int symbol_tree_init(symbol_table_t* symtab)
{
    sym_node_t* list = NULL;
    symbol_t* sym;
    sym_node_t* nptr;

    if ((sym = symtab->first) == NULL) {
	printf("warning: no symbols installed in symbol_tree_init\n");
	return -1;
    }
    while(sym) {
	if (sym->flags & SYMBOL_FLAG_INSTR) {
	    nptr = new_sym_node();
	    nptr->sym = sym;
	    nptr->next = list;
	    list = nptr;
	}
	sym = sym->next;
    }
    if (new_sym_node_array(&symtab->root, list, 0, 0) == NULL)
	return -1;
    return 0;
}

symbol_t* symbol_tree_lookup(sym_node_t* np, uint32_t ins)
{
    while(np && np->nbuckets) {
	int index = (ins >> np->shr) & (np->mask);
	np = np->bucket[index];
    }
    if (np) {
	symbol_t* sym;
	if (np->flags & SYMBOL_FLAG_NODE) {
	    while(np) {
		sym = np->sym;
		if ((ins & sym->mask) == sym->code)
		    return sym;
		np = np->next;
	    }
	    return NULL;
	}
	else {
	    sym = np->sym;
	    if ((ins & sym->mask) == sym->code)
		return sym;
	    return NULL;
	}
    }
    return NULL;
}

symbol_t* symbol_lookup(symbol_table_t* symtab, uint32_t ins)
{
    if (symtab->root)
	return symbol_tree_lookup(symtab->root, ins);
    else {
	symbol_t* sym;
	for (sym = symtab->first; sym; sym = sym->next) {
	    if (sym->flags & SYMBOL_FLAG_INSTR) {
		if ((ins & sym->mask) == sym->code)
		    return sym;
	    }
	}
	return NULL;
    }
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
	else if (sym->flags & SYMBOL_FLAG_INSTR) {
	    if (OPCODE_IS_16BIT(sym->code)) {
		fprintf(out, "%s/%d mask = %04x, code=%04x\n",
			sym->name, 
			__builtin_popcount(sym->mask),
			sym->mask,
			sym->code);
	    }
	    else {
		fprintf(out, "%s/%d mask = %08x, code=%08x\n",
			sym->name,
			__builtin_popcount(sym->mask),
			sym->mask,
			sym->code);
	    }
	}
	else {
	    fprintf(out, "%s = %u\n", sym->name, sym->value);
	}
    }
}
