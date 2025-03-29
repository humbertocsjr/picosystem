OUT = stage2.app stage1.boot
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.s: ../include/stdlib.bpp ../include/standalone.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.asm
	@echo [AS] $@
	@$(AS) -o $@ $<

%.app: %.o ../lib/libstandalone.a Makefile
	@echo [LD] $@
	@$(LD) -f picosystem -s 5120 -o $@ -L ../lib -l libstandalone.a $<

%.boot: %.o
	@echo [LD] $@
	@$(LD) -f pcboot -o $@ $<