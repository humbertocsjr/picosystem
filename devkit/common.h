#ifndef COMMON_H
#define COMMON_H


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#pragma pack(1)

#define SEGMENT_HEADER 0
#define SEGMENT_TEXT 1
#define SEGMENT_DATA 2
#define SEGMENT_BSS 3
#define SEGMENT_SYMBOLS 4
#define segment_t int

struct section
{
    struct section *next;
    segment_t segment;
    char * name;
    int32_t size;
};
#define section_t struct section_t

// CS | DS=ES=SS
#define AOUT_SMALL_V7 0411
// CS=DS=ES=SS
#define AOUT_TINY_V7 0407
// CS | DS=ES=SS
#define AOUT_HC_IX_EXEC 0511
#define AOUT_HC_IX_OBJ 0500

struct aout
{
    uint16_t signature;
    uint16_t text;
    uint16_t data;
    uint16_t bss;
    uint16_t syms;
    uint16_t entry;
    uint16_t pad;
    uint16_t relocs;
};
#define aout_t struct aout

#define AOUT_MASK_OFFSET 0xff
#define AOUT_MASK_SYMTYPE 0x300
#define AOUT_MASK_SEG 0xf000
#define AOUT_SYMTYPE_TEXTSEG 0x1000
#define AOUT_SYMTYPE_DATASEG 0x2000
#define AOUT_SYMTYPE_BSSSEG 0x4000
#define AOUT_SYMTYPE_GLOBAL 0x0100
#define AOUT_SYMTYPE_REFERENCE 0x0200
#define AOUT_SYMTYPE_BYTE_OFFSET 0x0400
#define AOUT_SYMTYPE_WORD_OFFSET 0x0800

struct aout_symbol
{
    char symbol[60];
    uint16_t type;
    uint16_t offset;
};
#define aout_symbol_t struct aout_symbol

#define AR_AOUT 0711
#define AR_TEXT 0721

struct ar
{
    uint16_t signature;
    uint16_t size;
    char name[60];
};
#define ar_t struct ar


#pragma pack()

char *copy(char *dst, char *src, size_t size);
char *concat(char *dst, char *src, size_t size);

#endif
