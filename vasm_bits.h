#ifndef __VASM_BITS_H__
#define __VASM_BITS_H__

#include <stddef.h> 
#include <stdint.h> 

#define bit_struct(type,fields)			\
    typedef struct  fields  type

#define unsigned_field(field, n)		\
    uint8_t field[n]

#define signed_field(field, n)			\
    int8_t field[n]

#define bit_sizeof(x) (sizeof((x))*8)
#define field_size(type,field) sizeof(((type*)0)->field)
#define field_mask(type,field) ((1 << (field_size(type,field)))-1)
#define sign_bit(type,field)   (1 << (field_size(type,field)-1))
#define field_pos(type,field)  offsetof(type,field)
#define field_next(type,field) (field_size(type,field)+field_pos(type,field))
#define bit_struct_sizeof(type) sizeof(type) // since we use array as template!

#define bitfield_fetch(type,field,reg)					\
    ( ((reg) >> field_pos(type,field)) & field_mask(type,field) )

#define bitfield_fetch_signed(type, field, reg)				\
    ((bitfield_fetch(type,field,reg) ^ sign_bit(type,field))-sign_bit(type,field))

#define bitfield_store(type,field,reg,value)				\
    ((((value) & field_mask(type,field)) << field_pos(type,field)) |	\
     ((reg) & ~(field_mask(type,field) << field_pos(type,field))))

#endif
