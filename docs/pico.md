# Pico System Software

TUI Operating System for IBM PC or Compatible.

### Features

- Cooperative Multitask
- Microkernel

# Pico System ABI Specs

### ABI Registers:

- AX: Command ID
- BX: Argument 0
- CX: Argument 1
- DX: Argument 2
- SI: Argument 3
- DI: Argument 4

### Interrupts

_Format: COMMAND(ARGUMENTS,...)_

- INT 0x80: Task Yield
- INT 0x81: Kernel
    - 0x00(segments): alloc memory segments
    - 0x01(segment): free memory segments
    - 0x02(): get free memory segments count
    - 0x10(filename): Create File
    - 0x11(filename): Open File
    - 0x12(file, buffer, size): Read File
    - 0x13(file, buffer, size): Write File
    - 0x14(file, offset): Seek from Start of File
    - 0x15(file, offset): Seek from Current Position of File
    - 0x20(service): Get a process that provides a service
    - 0x21(service): Bind a service to the process
    - 0x30(proc, service, buffer, size): Send request
    - 0x31(): Wait request
    - 0x32(buffer, size): Receive a request
    - 0x33(): Get request process id
    - 0x34(value, buffer, size): Send response
    - 0x40(filename, arguments): Execute file