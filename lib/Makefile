OUT = libstandalone.a libdos.a libpicosystem.a
BPP = ../devkit/i86-bpp
AS = ../devkit/i86-as
LD = ../devkit/i86-ld
AR = ../devkit/i86-ar

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT) $(wildcard common/*.o)  $(wildcard standalone/*.o)

%.s: ../include/stdlib.bpp %.bpp
	@echo [BPP] $@
	@$(BPP) $@ $^

%.o: %.s
	@echo [AS] $@
	@$(AS) -o $@ $<

libstandalone.a: $(patsubst %.bpp,%.o,$(wildcard common/*.bpp) $(wildcard standalone/*.bpp))
	@echo [AR] $@
	@$(AR) -f $@ -a $^

libdos.a: $(patsubst %.bpp,%.o,$(wildcard common/*.bpp) $(wildcard dos/*.bpp))
	@echo [AR] $@
	@$(AR) -f $@ -a $^

libpicosystem.a: $(patsubst %.bpp,%.o,$(wildcard common/*.bpp) $(wildcard picosystem/*.bpp))
	@echo [AR] $@
	@$(AR) -f $@ -a $^