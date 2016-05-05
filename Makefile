
OBJS = \
	vasm_symbol.o \
	vasm_scan.o \
	vasm_ccode.o \
	vasm_disasm.o \
	vasm_asm.o \
	vasm_emu.o \
	version.o \
	vasm.o

CFLAGS += -g -MD

vasm:	$(OBJS)
	gcc -o $@ $(OBJS)

include version.mk
-include $(OBJS:.o=.d)
