OUT = stage2.sys stage1.boot
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.s: ../include/stdlib.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.asm
	@echo [AS] $@
	@$(AS) -o $@ $<

%.sys: %.o ../lib/libstandalone.a
	@echo [LD] $@
	@$(LD) -f sys -o $@ -L ../lib -l libstandalone.a $<

%.boot: %.o
	@echo [LD] $@
	@$(LD) -f pcboot -o $@ $<