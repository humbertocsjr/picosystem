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
ifeq "$(wildcard $(KEXT0))" "$@" 
	cp $@ 0.kext
endif
ifeq "$(wildcard $(KEXT1))" "$@"
	cp $@ 1.kext
endif
ifeq "$(wildcard $(KEXT2))" "$@"
	cp $@ 2.kext
endif
ifeq "$(wildcard $(KEXT3))" "$@"
	cp $@ 3.kext
endif
ifeq "$(wildcard $(KEXT4))" "$@"
	cp $@ 4.kext
endif
ifeq "$(wildcard $(KEXT5))" "$@"
	cp $@ 5.kext
endif
ifeq "$(wildcard $(KEXT6))" "$@"
	cp $@ 6.kext
endif
ifeq "$(wildcard $(KEXT7))" "$@"
	cp $@ 7.kext
endif
ifeq "$(wildcard $(KEXT8))" "$@"
	cp $@ 8.kext
endif
ifeq "$(wildcard $(KEXT9))" "$@"
	cp $@ 9.kext
endif
