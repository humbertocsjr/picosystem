NAME = obj
OUT = $(patsubst %,../i86-%,$(NAME)) 
SDK = $(OUT) $(patsubst %,../bin32/i86-%,$(NAME))  $(patsubst %,../bin64/i86-%,$(NAME)) $(patsubst %,../binarm64/i86-%,$(NAME)) $(patsubst %,../binwin32/i86-%.exe,$(NAME))  $(patsubst %,../binwin64/i86-%.exe,$(NAME))

all: $(OUT)
	@echo >> /dev/null

sdk: $(SDK)
	@echo >> /dev/null

clean:
	@rm -f $(SDK)

../i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../bin32/i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@i686-linux-gnu-gcc -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../bin64/i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -m64 -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../binarm64/i86-%: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@aarch64-linux-gnu-gcc -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../binwin32/i86-%.exe: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@i686-w64-mingw32-gcc -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c

../binwin64/i86-%.exe: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@x86_64-w64-mingw32-gcc -o $@ $(wildcard *.c) ../hosts/i86-unx.c ../common.c