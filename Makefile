MAKEDIR = @make --no-print-directory $@ -C

all clean:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib
	$(MAKEDIR) boot
	$(MAKEDIR) kernel
	$(MAKEDIR) kexts
	$(MAKEDIR) apps
	$(MAKEDIR) distro
	@make --no-print-directory main-$@

main-all:
	@echo >> /dev/null
main-clean:
	@rm -f *.deb *.rpm *.zip test.s

sdk:
	$(MAKEDIR) devkit
	$(MAKEDIR) lib

packages: sdk
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
	@install -d devkit-deb/DEBIAN
	@install devkit/debpkgarm64 devkit-deb/DEBIAN/control
	@install -d devkit-deb/usr/local/bin
	@install devkit/binarm64/* devkit-deb/usr/local/bin
	@install -d devkit-deb/usr/local/picosdk
	@install -d devkit-deb/usr/local/picosdk/lib
	@install lib/*.a devkit-deb/usr/local/picosdk/lib/
	@install -d devkit-deb/usr/local/picosdk/include
	@install include/*.bpp devkit-deb/usr/local/picosdk/include/
	@dpkg-deb --root-owner-group --build devkit-deb picosdk.aarch64.deb
	@rm -Rf devkit-deb 
	@alien --to-rpm picosdk.aarch64.deb
	@cd devkit/binwin32; zip -q9r ../../picosdk.win32.zip *
	@zip -q9ru picosdk.win32.zip include/* lib/*
	@cd devkit/binwin64; zip -q9r ../../picosdk.win64.zip *
	@zip -q9ru picosdk.win64.zip include/* lib/*

install: sdk
	@install -d /usr/local/bin
	@install devkit/i86-* /usr/local/bin
	@install devkit/picofs /usr/local/bin
	@install -d /usr/local/picosdk
	@install -d /usr/local/picosdk/lib
	@install lib/*.a /usr/local/picosdk/lib/
	@install -d /usr/local/picosdk/include
	@install include/*.bpp /usr/local/picosdk/include/

run run_floppy run_1440: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/1440.img,format=raw,if=floppy,media=disk

run_360: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/360.img,format=raw,if=floppy,media=disk

run_720: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/720.img,format=raw,if=floppy,media=disk

run_1200: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/1200.img,format=raw,if=floppy,media=disk

run_2880: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/2880.img,format=raw,if=floppy,media=disk

run_hd: all
	@qemu-system-i386 -debugcon stdio -m 4 -drive file=distro/10404.img,format=raw,if=ide,media=disk

debug: all
	@bochs -debugger