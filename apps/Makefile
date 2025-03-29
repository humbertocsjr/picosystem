OUT = init.app
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.s: ../include/stdlib.bpp ../include/picosystem.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

%.o: %.asm
	@echo [AS] $@
	@$(AS) -o $@ $<

%.app: %.o ../lib/libpicosystem.a
	@echo [LD] $@
	@$(LD) -f picosystem -o $@ -L ../lib -l libpicosystem.a $<
