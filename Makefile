MAKEDIR = @make --no-print-directory $@ -C

all clean:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib
	$(MAKEDIR) boot
	$(MAKEDIR) distro
	@make --no-print-directory main-$@

main-all:
	@echo >> /dev/null
main-clean:
	@rm -f *.deb *.rpm

sdk:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib


deb: sdk
	@install -d devkit-deb/DEBIAN
	@install devkit/debpkg32 devkit-deb/DEBIAN/control
	@install -d devkit-deb/usr/local/bin
	@install devkit/bin32/* devkit-deb/usr/local/bin
	@install -d devkit-deb/usr/local/picosdk
	@install -d devkit-deb/usr/local/picosdk/lib
	@install lib/*.a devkit-deb/usr/local/picosdk/lib/
	@install -d devkit-deb/usr/local/picosdk/include
	@install include/*.bpp devkit-deb/usr/local/picosdk/include/
	@dpkg-deb --root-owner-group --build devkit-deb picosdk.i386.deb
	@rm -Rf devkit-deb 
	@alien --to-rpm picosdk.i386.deb
	@install -d devkit-deb/DEBIAN
	@install devkit/debpkg64 devkit-deb/DEBIAN/control
	@install -d devkit-deb/usr/local/bin
	@install devkit/bin64/* devkit-deb/usr/local/bin
	@install -d devkit-deb/usr/local/picosdk
	@install -d devkit-deb/usr/local/picosdk/lib
	@install lib/*.a devkit-deb/usr/local/picosdk/lib/
	@install -d devkit-deb/usr/local/picosdk/include
	@install include/*.bpp devkit-deb/usr/local/picosdk/include/
	@dpkg-deb --root-owner-group --build devkit-deb picosdk.amd64.deb
	@rm -Rf devkit-deb 
	@alien --to-rpm picosdk.amd64.deb

install: sdk
	@install -d /usr/local/bin
	@install devkit/i86-* /usr/local/bin
	@install devkit/picofs /usr/local/bin
	@install -d /usr/local/picosdk
	@install -d /usr/local/picosdk/lib
	@install lib/*.a /usr/local/picosdk/lib/
	@install -d /usr/local/picosdk/include
	@install include/*.bpp /usr/local/picosdk/include/