include ../conf.mk
ifneq ($(KEXT0),)
OUTK0 = 0.kext
endif
ifneq ($(KEXT1),)
OUTK1 = 1.kext
endif
ifneq ($(KEXT2),)
OUTK2 = 2.kext
endif
ifneq ($(KEXT3),)
OUTK3 = 3.kext
endif
ifneq ($(KEXT4),)
OUTK4 = 4.kext
endif
ifneq ($(KEXT5),)
OUTK5 = 5.kext
endif
ifneq ($(KEXT6),)
OUTK6 = 6.kext
endif
ifneq ($(KEXT7),)
OUTK7 = 7.kext
endif
ifneq ($(KEXT8),)
OUTK8 = 8.kext
endif
ifneq ($(KEXT9),)
OUTK9 = 9.kext
endif
ifneq ($(KEXT10),)
OUTK10 = 10.kext
endif
ifneq ($(KEXT11),)
OUTK11 = 11.kext
endif
ifneq ($(KEXT12),)
OUTK12 = 12.kext
endif
ifneq ($(KEXT13),)
OUTK13 = 13.kext
endif
ifneq ($(KEXT14),)
OUTK14 = 14.kext
endif
ifneq ($(KEXT15),)
OUTK15 = 15.kext
endif
OUT = $(patsubst %.bpp,%.app,$(wildcard *.bpp)) $(OUTK0) $(OUTK1) $(OUTK2) $(OUTK3) $(OUTK4) $(OUTK5) $(OUTK6) $(OUTK7) $(OUTK8) $(OUTK9) $(OUTK10) $(OUTK11) $(OUTK12) $(OUTK13) $(OUTK14) $(OUTK15)

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
	@$(LD) -f picosystem -s 4096 -o $@ -L ../lib -l libpicosystem.a $<

0.kext: $(KEXT0)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
1.kext: $(KEXT1)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
2.kext: $(KEXT2)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
3.kext: $(KEXT3)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
4.kext: $(KEXT4)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
5.kext: $(KEXT5)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
6.kext: $(KEXT6)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
7.kext: $(KEXT7)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
8.kext: $(KEXT8)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
9.kext: $(KEXT9)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
10.kext: $(KEXT10)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
11.kext: $(KEXT11)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
12.kext: $(KEXT12)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
13.kext: $(KEXT13)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
14.kext: $(KEXT14)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@
	
15.kext: $(KEXT15)
	@echo [KEXT] $@ "<=" $<
	@cp $< $@

