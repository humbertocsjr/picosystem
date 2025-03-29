include ../conf.mk
OUT = 360.img 720.img 1200.img 1440.img 2880.img 10404.img
PICOFS = ../devkit/picofs
BOOTLOADER = ../boot/stage1.boot
STAGE2 = ../boot/stage2.app
KERNEL = ../kernel/kernel.app
KEXTS = $(patsubst %,../kexts/%,$(KEXT0) $(KEXT1) $(KEXT2) $(KEXT3) $(KEXT4) $(KEXT5) $(KEXT6) $(KEXT7) $(KEXT8) $(KEXT9))
INIT = ../apps/init.app

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.img: $(PICOFS) $(BOOTLOADER) $(STAGE2) $(KERNEL) $(INIT) $(KEXTS) Makefile
	@echo [PICOFS] $@
	@$(PICOFS) $@ genfs $(patsubst %.img,%,$@) "System" $(BOOTLOADER)
	@$(PICOFS) $@ add $(STAGE2) /Loader
	@$(PICOFS) $@ mkdir /System
	@$(PICOFS) $@ mkdir /Users
	@$(PICOFS) $@ mkdir /Volumes
	@$(PICOFS) $@ mkdir /Applications
	@$(PICOFS) $@ add $(BOOTLOADER) /System/Loader/Stage1
	@$(PICOFS) $@ add $(STAGE2) /System/Loader/Stage2
	@$(PICOFS) $@ add $(KERNEL) /System/Kernel
	@$(PICOFS) $@ add_app $(INIT) /System/Init
ifneq "$(wildcard ../kexts/0.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/0.kext /System/Extensions/0.kext
endif
ifneq "$(wildcard ../kexts/1.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/1.kext /System/Extensions/1.kext
endif
ifneq "$(wildcard ../kexts/2.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/2.kext /System/Extensions/2.kext
endif
ifneq "$(wildcard ../kexts/3.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/3.kext /System/Extensions/3.kext
endif
ifneq "$(wildcard ../kexts/4.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/4.kext /System/Extensions/4.kext
endif
ifneq "$(wildcard ../kexts/5.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/5.kext /System/Extensions/5.kext
endif
ifneq "$(wildcard ../kexts/6.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/6.kext /System/Extensions/6.kext
endif
ifneq "$(wildcard ../kexts/7.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/7.kext /System/Extensions/7.kext
endif
ifneq "$(wildcard ../kexts/8.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/8.kext /System/Extensions/8.kext
endif
ifneq "$(wildcard ../kexts/9.kext)" ""
	@$(PICOFS) $@ add_app ../kexts/9.kext /System/Extensions/9.kext
endif
