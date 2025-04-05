# Pico Softwares

- [Pico System Software](docs/pico.md) \
    Operating System for IBM PC
- [Pico Software Development Kit](docs/sdk.md) \
    SDK for 8086 \
    Packages:
    - .deb: Debian/Ubuntu
    - .rpm: Fedora/Red Hat
    - .zip: Windows
- [B++ Language](docs/bpp.md) \
    Modernized B Language for RetroComputing Projects
- [Pico System Font](docs/pico.md) \
    ![character set](docs/charmap.png)

## How to generate Pico SDK/B++ Packages

- Install C Development Kit/NASM for your linux
    - Fedora
        ```sh
        sudo dnf copr enable lantw44/aarch64-linux-gnu-toolchain 
        sudo dnf install @c-development @development-tools nasm dpkg libstdc++.i686 glibc-devel.i686 alien mingw32-gcc mingw64-gcc aarch64-linux-gnu-binutils aarch64-linux-gnu-gcc aarch64-linux-gnu-glibc libgnat-devel
        ```
    - Debian/Ubuntu
        ```sh
        sudo apt install build-essential nasm crossbuild-essential-arm64 crossbuild-essential-i386 mingw-w64 mingw-w64-i686-dev  mingw-w64-x86-64-dev alien zip
        ```
- Generate packages
    ```sh
    make packages
    ```

## How to install Pico SDK/B++ Compiler
<a id="sdk-prereqs"></a>
- Install C Development Kit/NASM for your linux
    - Fedora
        ```sh
        sudo dnf copr enable lantw44/aarch64-linux-gnu-toolchain 
        sudo dnf install @c-development @development-tools nasm dpkg libstdc++.i686 glibc-devel.i686 alien mingw32-gcc mingw64-gcc aarch64-linux-gnu-binutils aarch64-linux-gnu-gcc aarch64-linux-gnu-glibc libgnat-devel
        ```
    - Debian/Ubuntu
        ```sh
        sudo apt install build-essential nasm crossbuild-essential-arm64 crossbuild-essential-i386 mingw-w64 mingw-w64-i686-dev  mingw-w64-x86-64-dev alien zip
        ```
- Compile Pico SDK
    ```sh
    make sdk
    ```
- Install Pico SDK
    ```sh
    sudo make install
    ```

## Run Pico System Software on Emulator

- Install [SDK/B++](#sdk-prereqs) prerequisites
- Install QEMU
- Execute emulator
```sh
make run
```
