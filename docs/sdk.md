# Pico Software Development Kit

SDK for Pico Softwares development.

## Tools

- i86-ar: Archiver
- i86-as: 8086 Assembler
- i86-bpp: B++ Compiler
- i86-ld: Linker
- i86-nm: Object Symbols Dump
- i86-size: Object Info

# How to install Pico SDK/B++ Compiler

- Install C Development Kit/NASM for your linux
    - Fedora
        ```sh
        sudo dnf install @c-development @development-tools nasm
        ```
    - Debian/Ubuntu
        ```sh
        sudo apt install build-essential nasm
        ```
- Compile Pico SDK
    ```sh
    make sdk
    ```
- Install Pico SDK
    ```sh
    sudo make install
    ```