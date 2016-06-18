OBJDIR = obj
BINDIR = .

VASM = $(BINDIR)/vasm

OBJS = \
	$(OBJDIR)/vasm_symbol.o \
	$(OBJDIR)/vasm_scan.o \
	$(OBJDIR)/vasm_ccode.o \
	$(OBJDIR)/vasm_disasm.o \
	$(OBJDIR)/vasm_asm.o \
	$(OBJDIR)/vasm_emu.o \
	$(OBJDIR)/vasm_rv32i.o \
	$(OBJDIR)/vasm_rv32m.o \
	$(OBJDIR)/vasm_rv32c.o \
	$(OBJDIR)/vasm_rv32f.o \
	$(OBJDIR)/version.o \
	$(OBJDIR)/vasm.o

CFLAGS += -Wall -g -MD -m32
LDFLAGS += -m32

$(VASM): $(OBJS)
	gcc $(LDFLAGS) -o $(VASM) $(OBJS)

clean:
	rm $(OBJS) $(VASM)

$(OBJDIR)/%.o:	%.c
	$(CC) -c $(CFLAGS) $< -o $@

include version.mk
-include $(OBJS:.o=.d)
