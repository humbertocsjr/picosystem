include ../conf.mk
OUT = $(patsubst %.bpp,%.app,$(wildcard *.bpp))

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f *.o $(OUT) *.kext *.app

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
	@if [[ "$(KEXT0)" == "$@" ]]; then cp $@ 0.kext; fi
	@if [[ "$(KEXT1)" == "$@" ]]; then cp $@ 1.kext; fi
	@if [[ "$(KEXT2)" == "$@" ]]; then cp $@ 2.kext; fi
	@if [[ "$(KEXT3)" == "$@" ]]; then cp $@ 3.kext; fi
	@if [[ "$(KEXT4)" == "$@" ]]; then cp $@ 4.kext; fi
	@if [[ "$(KEXT5)" == "$@" ]]; then cp $@ 5.kext; fi
	@if [[ "$(KEXT6)" == "$@" ]]; then cp $@ 6.kext; fi
	@if [[ "$(KEXT7)" == "$@" ]]; then cp $@ 7.kext; fi
	@if [[ "$(KEXT8)" == "$@" ]]; then cp $@ 8.kext; fi
	@if [[ "$(KEXT9)" == "$@" ]]; then cp $@ 9.kext; fi

