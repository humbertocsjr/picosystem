#include "as.h"

FILE *_out = 0;
char *_out_name = 0;
segment_t _seg = SEGMENT_HEADER;
segment_t _out_seg = -1;

void open_out(char *filename)
{
    unlink(filename);
    _out = fopen(filename, "wb");
    _out_name = add_name(filename);
    if(!_out)
    {
        error("can't open output file: %s", filename);
    }
}

void remove_out()
{
    if(_out && _out_name)
    {
        fclose(_out);
        unlink(_out_name);
    }
}

void close_out()
{
    fclose(_out);
}

void set_output_segment(segment_t seg)
{
    _out_seg = seg;
}

void set_segment(segment_t seg)
{
    _seg = seg;
}

segment_t get_segment()
{
    return _seg;
}

int can_output()
{
    return _out_seg != -1;
}

void reserve(int size)
{
    if(_seg == SEGMENT_TEXT) _header.text+=size;
    if(_seg == SEGMENT_DATA) _header.data+=size;
    if(_seg == SEGMENT_BSS) _header.bss+=size;
    if(_seg != _out_seg) return;
    if(_seg != SEGMENT_BSS)
    {
        while(size-- > 0)
        {
            fputc(0, _out);
        }
    }
}

void outb(int8_t value)
{
    if(_seg == SEGMENT_TEXT) _header.text++;
    if(_seg == SEGMENT_DATA) _header.data++;
    if(_seg == SEGMENT_BSS) error_at(curr(), "invalid segment.");
    if(_seg != _out_seg) return;
    fwrite(&value, 1, 1, _out);
}

void outw(int16_t value)
{
    if(_seg == SEGMENT_TEXT) _header.text+=2;
    if(_seg == SEGMENT_DATA) _header.data+=2;
    if(_seg == SEGMENT_BSS) error_at(curr(), "invalid segment.");
    if(_seg != _out_seg) return;
    fwrite(&value, 1, 2, _out);
}

void outd(int32_t value)
{
    if(_seg == SEGMENT_TEXT) _header.text+=4;
    if(_seg == SEGMENT_DATA) _header.data+=4;
    if(_seg == SEGMENT_BSS) error_at(curr(), "invalid segment.");
    if(_seg != _out_seg) return;
    fwrite(&value, 1, 4, _out);
}

void outraw(void *data, int size)
{
    fwrite(data, 1, size, _out);
}

void outreference(symbol_t *sym, int32_t pointer, int32_t offset)
{
    aout_symbol_t aout_sym;
    _header.syms++;
    if(SEGMENT_SYMBOLS != _out_seg) return;
    aout_sym.type = AOUT_SYMTYPE_REFERENCE;
    if(_seg == SEGMENT_TEXT) aout_sym.type |= AOUT_SYMTYPE_TEXTSEG;
    if(_seg == SEGMENT_DATA) aout_sym.type |= AOUT_SYMTYPE_DATASEG;
    if(_seg == SEGMENT_BSS) aout_sym.type |= AOUT_SYMTYPE_BSSSEG;
    aout_sym.type |= offset & AOUT_MASK_OFFSET;
    aout_sym.offset = pointer;
    strncpy(aout_sym.symbol, sym->name, 60);
    fwrite(&aout_sym, 1, sizeof(aout_symbol_t), _out);
}

void outdispreference(symbol_t *sym, int32_t pointer, int32_t offset, int size)
{
    aout_symbol_t aout_sym;
    _header.syms++;
    if(SEGMENT_SYMBOLS != _out_seg) return;
    aout_sym.type = AOUT_SYMTYPE_REFERENCE | (size == 1 ? AOUT_SYMTYPE_BYTE_OFFSET : AOUT_SYMTYPE_WORD_OFFSET);
    if(_seg == SEGMENT_TEXT) aout_sym.type |= AOUT_SYMTYPE_TEXTSEG;
    if(_seg == SEGMENT_DATA) aout_sym.type |= AOUT_SYMTYPE_DATASEG;
    if(_seg == SEGMENT_BSS) aout_sym.type |= AOUT_SYMTYPE_BSSSEG;
    aout_sym.type |= offset & AOUT_MASK_OFFSET;
    aout_sym.offset = pointer;
    strncpy(aout_sym.symbol, sym->name, 60);
    fwrite(&aout_sym, 1, sizeof(aout_symbol_t), _out);
}

void outreflocal(symbol_t *sym, int32_t pointer, int32_t offset)
{
    aout_symbol_t aout_sym;
    _header.syms++;
    if(SEGMENT_SYMBOLS != _out_seg) return;
    aout_sym.type = AOUT_SYMTYPE_REFERENCE;
    if(_seg == SEGMENT_TEXT)
    {
        aout_sym.type |= AOUT_SYMTYPE_TEXTSEG;
    }
    if(sym->seg == SEGMENT_TEXT)
    {
        strncpy(aout_sym.symbol, "__textoff", 60);
    }
    if(_seg == SEGMENT_DATA)
    {
        aout_sym.type |= AOUT_SYMTYPE_DATASEG;
    }
    if(sym->seg == SEGMENT_DATA)
    {
        strncpy(aout_sym.symbol, "__dataoff", 60);
    }
    if(_seg == SEGMENT_BSS)
    {
        aout_sym.type |= AOUT_SYMTYPE_BSSSEG;
    }
    if(sym->seg == SEGMENT_BSS)
    {
        strncpy(aout_sym.symbol, "__bssoff", 60);
    }
    aout_sym.type |= offset & AOUT_MASK_OFFSET;
    aout_sym.offset = pointer;
    fwrite(&aout_sym, 1, sizeof(aout_symbol_t), _out);
}

void outdispreflocal(symbol_t *sym, int32_t pointer, int32_t offset, int size)
{
    aout_symbol_t aout_sym;
    _header.syms++;
    if(SEGMENT_SYMBOLS != _out_seg) return;
    aout_sym.type = AOUT_SYMTYPE_REFERENCE | (size == 1 ? AOUT_SYMTYPE_BYTE_OFFSET : AOUT_SYMTYPE_WORD_OFFSET);
    if(_seg == SEGMENT_TEXT)
    {
        aout_sym.type |= AOUT_SYMTYPE_TEXTSEG;
    }
    if(sym->seg == SEGMENT_TEXT)
    {
        strncpy(aout_sym.symbol, "__textoff", 60);
    }
    if(_seg == SEGMENT_DATA)
    {
        aout_sym.type |= AOUT_SYMTYPE_DATASEG;
    }
    if(sym->seg == SEGMENT_DATA)
    {
        strncpy(aout_sym.symbol, "__dataoff", 60);
    }
    if(_seg == SEGMENT_BSS)
    {
        aout_sym.type |= AOUT_SYMTYPE_BSSSEG;
    }
    if(sym->seg == SEGMENT_BSS)
    {
        strncpy(aout_sym.symbol, "__bssoff", 60);
    }
    aout_sym.type |= offset & AOUT_MASK_OFFSET;
    aout_sym.offset = pointer;
    fwrite(&aout_sym, 1, sizeof(aout_symbol_t), _out);
}

void outpublic(symbol_t *sym)
{
    aout_symbol_t aout_sym;
    _header.syms++;
    if(SEGMENT_SYMBOLS != _out_seg) return;
    aout_sym.type = AOUT_SYMTYPE_GLOBAL;
    if(_seg == SEGMENT_TEXT) aout_sym.type |= AOUT_SYMTYPE_TEXTSEG;
    if(_seg == SEGMENT_DATA) aout_sym.type |= AOUT_SYMTYPE_DATASEG;
    if(_seg == SEGMENT_BSS) aout_sym.type |= AOUT_SYMTYPE_BSSSEG;
    aout_sym.offset = sym->value;
    strncpy(aout_sym.symbol, sym->name, 60);
    fwrite(&aout_sym, 1, sizeof(aout_symbol_t), _out);
}

int get_offset()
{
    if(_seg == SEGMENT_TEXT) return _header.text;
    if(_seg == SEGMENT_DATA) return _header.data;
    if(_seg == SEGMENT_BSS) return _header.bss;
    return 0;
}
