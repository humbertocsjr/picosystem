OUT = 1440.img
PICOFS = ../devkit/picofs
BOOTLOADER = ../boot/stage1.boot

all: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

%.img: $(PICOFS) $(BOOTLOADER)
	@echo [PICOFS] $@ 
	@$(PICOFS) $@ genfs $(patsubst %.img,%,$@) "System" $(BOOTLOADER)