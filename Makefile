
OBJS = \
	vasm_symbol.o \
	vasm_scan.o \
	vasm_ccode.o \
	vasm_disasm.o \
	vasm_asm.o \
	vasm_emu.o \
	vasm.o

CFLAGS = -g

vasm:	$(OBJS)
	gcc -o $@ $(OBJS)

vasm_symbol.o:	vasm_symbol.c vasm_symbol.h
vasm_ccode.o:	vasm_ccode.c vasm_ccode.h vasm.h
vasm_disasm.o:	vasm_disasm.c vasm.h
vasm_asm.o:	vasm_asm.c vasm.h
vasm_emu.o:	vasm_emu.c vasm.h
vasm_scan.o:	vasm_scan.c vasm_scan.h vasm.h
vasm.o:		vasm.c vasm.h

vasm.h:		vasm_types.h vasm_symbol.h