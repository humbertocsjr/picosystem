OUT = ../picofs

all sdk: $(OUT)
	@echo >> /dev/null

clean:
	@rm -f $(OUT)

../picofs: $(wildcard *.c) $(wildcard *.h) $(wildcard ../*.c)  $(wildcard ../*.h) $(wildcard ../hosts/*.c)  $(wildcard ../hosts/*.h)
	@echo [CC] $@
	@cc -g -o $@ $(wildcard *.c) ../common.c 
