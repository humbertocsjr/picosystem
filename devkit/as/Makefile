NAME = as
OUT = $(patsubst %,../i86-%,$(NAME)) $(patsubst %,../bin32/i86-%,$(NAME))  $(patsubst %,../bin64/i86-%,$(NAME))

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

../i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c target/i86.c

../bin32/i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -m32 -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c target/i86.c

../bin64/i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -m64 -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c target/i86.c