# Pico Software Development Kit

Pico System Software/DOS SDK.

## Targets

- i86 Architeture
    - DOS 2 or Compatible
    - Pico System Software
    - Standalone (BIOS calls)

## Tools

- i86-ar: [Archiver](#sdk-ar)
- i86-as: [8086 Assembler](#sdk-as)
- i86-bpp: [B++ Compiler](bpp.md)
- i86-ld: [Linker](#sdk-ld)
- i86-nm: [Object Symbols Dump](#sdk-nm)
- i86-size: [Object Info](#sdk-size)
- picofs: [Pico File System Tool](#sdk-picofs)

## Archiver
<a id="sdk-ar"></a>

```
Usage: i86-ar [-f archive] [-a file] [-l]
Options:
 -f archive      set archive file
 -a file         append file to archive
 -l              list all files from archive
```

## Assembler
<a id="sdk-as"></a>

```
Usage: i86-as [-o output] [objects...]
Options:
 -o output       set output file
```

## Linker
<a id="sdk-ld"></a>

```
Usage: i86-ld [-f format] [-b] [-o output] [-L dir] [-l library] [-s stack_size] [objects...]
Options:
 -f format       set output format (see list bellow)
 -b              include bss space to output
 -o output       set output file
 -s stack_size   set stack size (default: 2048)
 -L dir          add to library dirs list
 -l library      add library (auto include 'lib' prefix)
Output format:
 picosystem|pico produce Pico System Software a.out
 v7|v7small      produce Seventh Edition UNIX a.out
 v7tiny          tiny model output (text/data/bss in one segment)
 com             DOS COM executable (ORG 0x100)
 sys             DOS SYS executable (ORG 0x0)
 pcboot          PC Boot Sector executable (ORG 0x7c00)
```

## Object Symbols Dump
<a id="sdk-nm"></a>

```
Usage: i86-nm [objects...]
```

## Object info
<a id="sdk-size"></a>

```
Usage: i86-size [objects...]
```

## Pico File System Tool
<a id="sdk-picofs"></a>

```
Usage: picofs [image] [command] [arguments ...]
Commands:
 genfs [size kib] [name] [bootloader]
 mkfs [size kib] [name] [bootloader]
 add [local file] [destination address]
 add_file [local file] [destination address]
 add_app [local file] [destination address]
 mkdir [address]
 df
```