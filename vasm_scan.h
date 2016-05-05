#ifndef __VASM_SCAN_H__
#define __VASM_SCAN_H__

#include "vasm_types.h"

#define TOKEN_SYMBOL        256
#define TOKEN_NUMBER        257

typedef struct _token_t {
    int c;             // token class
    char* name;        // token characters
} token_t;

extern int scan(char* ptr, char* dst, token_t* tokens, size_t max_tokens);

#endif
