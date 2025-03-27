MAKEDIR = @make --no-print-directory $@ -C

all clean:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib
	$(MAKEDIR) boot
	$(MAKEDIR) distro

sdk:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib


install: sdk
	@install -d /usr/local/bin
	@install devkit/i86-* /usr/local/bin
	@install devkit/picofs /usr/local/bin
	@install -d /usr/local/picosdk
	@install -d /usr/local/picosdk/lib
	@install lib/*.a /usr/local/picosdk/lib/
	@install -d /usr/local/picosdk/include
	@install include/*.bpp /usr/local/picosdk/include/