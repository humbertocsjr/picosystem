# Pico Softwares

- [Pico System Software](docs/pico.md) \
    Operating System for IBM PC
- [Pico Software Development Kit](docs/sdk.md) \
    SDK for 8086
- [B++ Language](docs/bpp.md) \
    Modernized B Language for RetroComputing Projects

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