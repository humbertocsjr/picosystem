OUT = ../i86-ld

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

../i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c target/i86.c
