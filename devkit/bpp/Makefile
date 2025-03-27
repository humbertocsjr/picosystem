OUT = ../i86-bpp

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

../i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c
