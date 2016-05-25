#ifndef __VASM_TYPES_H__
#define __VASM_TYPES_H__

#include <stdlib.h>
#include <stdint.h>

#define sizeof_array(t) (sizeof(t)/sizeof(t[0]))

#define    RV32I 0x00000001
#define    RV32M 0x00000002
// #define RV32E 0x00000004
// #define RV32A 0x00000008
// #define RV32F 0x00000010
// #define RV32D 0x00000020
// #define    RV32C 0x01000000

// #define RV64I 0x00010000
// #define RV64M 0x00020000
// #define RV64A 0x00040000
// #define RV64F 0x00100000
// #define RV64D 0x00200000
// #define RV64C 0x02000000

#endif
