OUT = kernel.app
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld
SRC = $(wildcard *.bpp)
OBJ = $(patsubst %.bpp,%.o,$(SRC))

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.s: ../include/stdlib.bpp ../include/standalone.bpp include/header.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.asm
	@echo [AS] $@
	@$(AS) -o $@ $<

%.app: $(OBJ) ../lib/libstandalone.a
	@echo [LD] $@
	@$(LD) -f picosystem -o $@ -L ../lib -l libstandalone.a $(OBJ)
