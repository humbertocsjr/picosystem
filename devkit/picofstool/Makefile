NAME = picofs
OUT = $(patsubst %,../%,$(NAME)) $(patsubst %,../bin32/%,$(NAME))  $(patsubst %,../bin64/%,$(NAME))

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

../$(NAME): $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../bin32/$(NAME): $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -m32 -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../bin64/$(NAME): $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -m64 -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c