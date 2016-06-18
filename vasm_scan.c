
#include <ctype.h>
#include "vasm_scan.h"

#define is_sign(x) ( ((x) == '-') || ((x) == '+'))
#define isbdigit(x) (((x) == '0') || ((x) == '1'))
#define isodigit(x) (((x) >= '0') || ((x) <= '7'))

static inline int xval(int c)
{
    if ((c >= '0') && (c <= '9')) return (c-'0');
    if ((c >= 'a') && (c <= 'f')) return ((c-'a')+10);
    if ((c >= 'A') && (c <= 'F')) return ((c-'A')+10);
    return -1;
}

static inline int oval(int c)
{
    if ((c >= '0') && (c <= '7')) return (c-'7');
    return -1;
}

// escaped character
char* escaped(char* ptr, int* cp)
{
    int c;
    switch((c = *ptr++)) {
    case 'a': *cp = '\a'; break;
    case 'b': *cp = '\b'; break;
    case 'f': *cp = '\f'; break;
    case 'n': *cp = '\n'; break;
    case 'r': *cp = '\r'; break;
    case 't': *cp = '\t'; break;
    case 'v': *cp = '\v'; break;
    case '\\': *cp = '\\'; break;
    case '\'': *cp = '\''; break;
    case '"': *cp = '"'; break;
    case '?': *cp = '?'; break;
    case 'x':
	if (isxdigit(*ptr)) {
	    c = xval(*ptr++);
	    while(isxdigit(*ptr))
		c = c*16 + xval(*ptr++);
	    *cp = c;
	}
	else {
	    *cp = 'x';
	    return ptr-1;
	}
	break;
    default:
	if (isodigit(*ptr)) {
	    c = oval(*ptr++);
	    while(isodigit(*ptr))
		c = c*8 + oval(*ptr++);
	    *cp = c;
	}
	else
	    *cp = *ptr++;
    }
    return ptr;
}

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
	if ((ptr[0]=='#') || (ptr[0]==';'))
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
	else if ((ptr[0] == '.') && isalpha(ptr[1])) {
	    tokens[i].c = TOKEN_COMMAND;
	    tokens[i].name = dst;
	    *dst++ = *ptr++;
	    while(isalpha(*ptr) || isdigit(*ptr) || (*ptr == '_'))
		*dst++ = *ptr++;
	    *dst++ = '\0';
	}
	else if (ptr[0] == '"') {
	    tokens[i].c = TOKEN_STRING;
	    tokens[i].name = dst;
	    ptr++;
	    while(*ptr && (*ptr != '"')) {
		if (*ptr == '\\') {
		    int c;
		    ptr = escaped(ptr+1, &c);
		    *dst++ = c;
		}
		else
		    *dst++ = *ptr++;
	    }
	    if (*ptr == '"')
		ptr++;
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
