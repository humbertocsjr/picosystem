OUT = kernel.app
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld
OBJFY = ../devkit/i86-obj
SRC = $(wildcard *.bpp) $(wildcard *.fon)
OBJ = $(patsubst %.fon,%.o,$(patsubst %.bpp,%.o,$(SRC)))

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT) *.o

%.s: ../include/stdlib.bpp ../include/standalone.bpp include/header.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.asm
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.fon
	@echo [OBJ] $@
	@$(OBJFY) -o $@ -b $< -n $(patsubst %.fon,_font_%,$<);

%.app: $(OBJ) ../lib/libstandalone.a
	@echo [LD] $@
	@$(LD) -f picosystem -o $@ -L ../lib -l libstandalone.a $(OBJ)
