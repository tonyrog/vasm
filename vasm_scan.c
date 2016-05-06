
#include <ctype.h>
#include "vasm_scan.h"

#define is_sign(x) ( ((x) == '-') || ((x) == '+'))
#define isbdigit(x) (((x) == '0') || ((x) == '1'))
// integer numbers:
//    [+|-] [0-9]+
//    [+|-] 0x [0-9A-Fa-f]+
//    [+|-] 0b [0-1]+
// symbols:
//    [a-zA-Z_][a-zA-Z0-9_.]*
//
int scan(char* ptr, char* dst, token_t* tokens, size_t max_tokens)
{
    int i = 0;

    while(*ptr && (i < max_tokens)) {
	while(isspace(*ptr)) ptr++;
	if ((ptr[0]=='/') && (ptr[1]=='/'))
	    return i;
	if (ptr[0]=='#')
	    return i;
	if (ptr[0]=='\0') 
	    return i;
	if (isalpha(*ptr) || (*ptr == '_')) {
	    tokens[i].c = TOKEN_SYMBOL;
	    tokens[i].name = dst;
	    while(isalpha(*ptr) || isdigit(*ptr) || 
		  (*ptr == '_') || (*ptr == '.'))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if (is_sign(ptr[0]) && (ptr[1] == '0') && (ptr[2] == 'x') &&
		 isxdigit(ptr[3])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    while(isxdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if ((ptr[0] == '0') && (ptr[1] == 'x') && isxdigit(ptr[2])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    while(isxdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if (is_sign(ptr[0]) && (ptr[1] == '0') && (ptr[2] == 'b') && 
		 isbdigit(ptr[3])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    while(isbdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if ((ptr[0] == '0') && (ptr[1] == 'b') && isbdigit(ptr[2])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    *dst++ = *ptr++;
	    while(isbdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if (isdigit(ptr[0])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    while(isdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';	    
	}
	else if (is_sign(ptr[0]) && isdigit(ptr[1])) {
	    tokens[i].c = TOKEN_NUMBER;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    while(isdigit(*ptr))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else {
	    tokens[i].c = *ptr;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    *dst++ = '\0';
	}
	i++;
    }
    return i;
}
