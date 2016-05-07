/*
 *  Code generation and emulation of vasm language
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <ctype.h>

#include "vasm.h"
#include "vasm_ccode.h"
#include "version.h"

#define MAX_TOKENS       1024
#define LINE_BUFFER_SIZE 1024

int vasm_ctx_init(vasm_ctx_t* ctx)
{
    memset(ctx->rt.reg, 0, sizeof(ctx->rt.reg));
    ctx->rt.mem_size = MEMORY_SIZE;
    ctx->rt.waddr = 0;
    ctx->rt.pc = 0;
    symbol_table_init(&ctx->symtab);
    ctx->filename = "";
    ctx->lineno = 0;
    ctx->debug = 0;
    ctx->verbose = 0;
    asm_init(&ctx->symtab);
    return 0;
}

int eq_suffix(char* ptr, char* name)
{
    if ((ptr = strrchr(ptr, '.')) == NULL)
	return 0;
    return (strcmp(ptr, name) == 0);
}

void usage(char* cmd)
{
    fprintf(stderr, "vasm: usage [options] command args\n");
    if ((strlen(cmd)==0) || (strcmp(cmd, "options") == 0)) {
	fprintf(stderr, "    options: -o <output-file>\n");
	fprintf(stderr, "             -t <output-type> C,vasm\n");
	fprintf(stderr, "             -d\n");
    }
}

int main(int argc, char** argv)
{
    FILE* f = stdin;
    vasm_ctx_t ctx;
    char* out_file = NULL;
    char* out_type = NULL;
    char* run_label = NULL;
    int c;

    vasm_ctx_init(&ctx);

    while((c = getopt(argc, argv, "vdt:o:r:")) != -1) {
	switch(c) {
	case 't':
	    out_type = optarg;
	    break;
	case 'o':
	    out_file = optarg;
	    break;
	case 'd':
	    ctx.debug = 1;
	    break;
	case 'v':
	    ctx.verbose = 1;
	    fprintf(stderr, "vasm version=%s short=%s, sha=%s\n",
		    vasm_version, git_tree_sha_short, git_tree_sha);
	    break;
	case 'r':
	    run_label = optarg;
	    break;
	default:
	    usage("options");
	}
    }

    if (out_type == NULL) {
	if (out_file != NULL) {
	    if (eq_suffix(out_file, ".c"))
		out_type = "c";
	    else if (eq_suffix(out_file, ".vasm"))
		out_type = "vasm";
	    else if (eq_suffix(out_file, ".s"))
		out_type = "asm";
	    else if (eq_suffix(out_file, ".o"))
		out_type = "obj";
	    else
		usage("");
	}
    }

    if (optind >= argc) {
	f = stdin;
	ctx.filename = "*stdin*";
    }
    else {
	ctx.filename = argv[optind];
	if ((f = fopen(ctx.filename, "r")) == NULL) {
	    fprintf(stderr, "vasm: unable to open %s\n", ctx.filename);
	    exit(1);
	}
    }

    if (ctx.debug) {
	fprintf(stderr, "sizeof(instr_r) = %lu\n", sizeof(instr_r));
	fprintf(stderr, "sizeof(instr_i) = %lu\n", sizeof(instr_i));
	fprintf(stderr, "sizeof(instr_s) = %lu\n", sizeof(instr_s));
	fprintf(stderr, "sizeof(instr_sb) = %lu\n", sizeof(instr_sb));
	fprintf(stderr, "sizeof(instr_u) = %lu\n", sizeof(instr_u));
	fprintf(stderr, "sizeof(instr_uj) = %lu\n", sizeof(instr_uj));

#if defined(RV32C)
	fprintf(stderr, "sizeof(instr_cr) = %lu\n", sizeof(instr_cr));
	fprintf(stderr, "sizeof(instr_ci) = %lu\n", sizeof(instr_ci));
	fprintf(stderr, "sizeof(instr_css) = %lu\n", sizeof(instr_css));
	fprintf(stderr, "sizeof(instr_ciw) = %lu\n", sizeof(instr_ciw));
	fprintf(stderr, "sizeof(instr_cl) = %lu\n", sizeof(instr_cl));
	fprintf(stderr, "sizeof(instr_cb) = %lu\n", sizeof(instr_cb));
	fprintf(stderr, "sizeof(instr_cj) = %lu\n", sizeof(instr_cj));
#endif
    }

    // assemble file
    while(fgets(ctx.linebuf, sizeof(ctx.linebuf), f)) {
	char token_data[LINE_BUFFER_SIZE*2];
	token_t tokens[MAX_TOKENS];
	size_t len = strlen(ctx.linebuf);
	int n;

	if (ctx.linebuf[len-1] == '\n')
	    ctx.linebuf[len-1] = '\0';
	ctx.lineno++;
#ifdef HARD_DEBUG
	if (ctx.debug) {
	    fprintf(stderr, "%s:%d: [%s]\n", 
		    ctx.filename, ctx.lineno, ctx.linebuf);
	}
#endif
	if ((n = scan(ctx.linebuf, token_data, tokens, MAX_TOKENS)) > 0) {
	    tokens[n].c = 0;
	    tokens[n].name = NULL;
	    assemble(&ctx, tokens, n);
	}
    }
    if (f != stdin)
	fclose(f);

    if (out_file == NULL)
	f = stdout;
    else if ((f = fopen(out_file, "w")) == NULL) {
	fprintf(stderr, "vasm: unable to open %s\n", out_file);
	exit(1);
    }

    if (out_type == NULL) 
	;
    else if (strcmp(out_type, "c") == 0) {
	gen_ccode(f, &ctx.symtab, ctx.rt.mem, ctx.rt.waddr);
    }
    else if (strcmp(out_type, "vasm") == 0) {
	disasm(f, &ctx.symtab, ctx.rt.mem, ctx.rt.waddr);
    }

    if (run_label != NULL) {
	symbol_t* sym = symbol_table_lookup(&ctx.symtab, run_label);
	if (sym == NULL) {
	    fprintf(stderr, "label %s not found\n", run_label);
	    exit(1);
	}
	if (!(sym->flags & SYMBOL_FLAG_LABEL)) {
	    fprintf(stderr, "%s is not a label\n", run_label);
	    exit(1);
	}
	run(stdout, &ctx.symtab, &ctx.rt, sym->value);
    }
    if (f != stdout)
	fclose(f);
    exit(0);
}
