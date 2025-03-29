MAKEDIR = @make --no-print-directory $@ -C

all clean sdk:
	@mkdir -p bin32
	@mkdir -p bin64
	@mkdir -p binwin32
	@mkdir -p binwin64
	@mkdir -p binarm64
	$(MAKEDIR) ar
	$(MAKEDIR) as
	$(MAKEDIR) bpp
	$(MAKEDIR) ld
	$(MAKEDIR) nm
	$(MAKEDIR) size
	$(MAKEDIR) picofstool
