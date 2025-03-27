#include "ld.h"

#ifdef OUT_BIN
void output_bin(int stack_size, int include_bss, int32_t bin_offset)
{
    symbol_t *entry_sym;
    int i, s, len, total;
    long text_offset, data_offset, bss_offset;
    long text_start, data_start, bss_start;
    int32_t size = bin_offset;
    int32_t offset = 0;
    int16_t temp;
    char buffer[BUFFER_MAX];
    aout_symbol_t aout_sym;
    entry_sym = find_symbol("_start");
    if(!entry_sym)
    {
        fprintf(stderr, "error: entry symbol (_start) not found.\n");
        if(_out)
        {
            fclose(_out);
            unlink(_out_name);
        }
        exit(1);
    }
    add_symbol("__segoff", 0);
    add_symbol("__etext", 0);
    add_symbol("__edata", 0);
    add_symbol("__end", 0);
    add_symbol("__textoff", 0);
    add_symbol("__dataoff", 0);
    add_symbol("__bssoff", 0);
    set_value("__etext", 3);
    set_value("__end", stack_size);
    make_symbol_tree(entry_sym);
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].name && _files[i].output_include)
        {
            add_value("__etext", _files[i].aout_header.text);
            add_value("__edata", _files[i].aout_header.data);
            add_value("__end", _files[i].aout_header.bss);
            size = get_value("__etext") + get_value("__edata") + get_value("__end");
            if(size >= 0xfde0)
            {
                fprintf(stderr, "text/data/bss segment size overflow. (%u | 0x%x)\n", (unsigned)size, size);
                fclose(_out);
                unlink(_out_name);
                exit(1);
            }
        }
    }
    add_value("__edata", get_value("__etext"));
    add_value("__end", get_value("__edata"));
    text_start = bin_offset + 3;
    data_start = bin_offset + get_value("__etext");
    bss_start = bin_offset + get_value("__edata");
    fputc(get_bin_jump(), _out);
    fputc(0x00, _out);
    fputc(0x00, _out);
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].name && _files[i].output_include)
        {
            total = _files[i].aout_header.text;
            fseek(_files[i].file, _files[i].aout_offset + sizeof(aout_t), SEEK_SET);
            do
            {
                len = fread(buffer, 1, total > BUFFER_MAX ? BUFFER_MAX : total, _files[i].file);
                if(len)
                {
                    fwrite(buffer, 1, len, _out);
                }
                total -= len;
            } while(total > 0 && len >= BUFFER_MAX);
        }
    }
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].name && _files[i].output_include)
        {
            total = _files[i].aout_header.data;
            fseek(_files[i].file, _files[i].aout_offset + sizeof(aout_t) + _files[i].aout_header.text, SEEK_SET);
            do
            {
                len = fread(buffer, 1, total > BUFFER_MAX ? BUFFER_MAX : total, _files[i].file);
                if(len)
                {
                    fwrite(buffer, 1, len, _out);
                }
                total -= len;
            } while(total > 0 && len >= BUFFER_MAX);
        }
    }
    if(include_bss)
    {
        fseek(_out, _aout.text + _aout.data + _aout.bss - 1, SEEK_SET);
        fputc(0, _out);
    }
    text_offset = text_start;
    data_offset = data_start;
    bss_offset = bss_start;
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].name && _files[i].output_include && _files[i].aout_header.signature == AOUT_HC_IX_OBJ)
        {
            fseek(_files[i].file, _files[i].aout_offset + _files[i].aout_header.text + _files[i].aout_header.data + sizeof(aout_t), SEEK_SET);
            for(s = 0; s < _files[i].aout_header.syms; s++)
            {
                fread(&aout_sym, 1, sizeof(aout_symbol_t), _files[i].file);
                switch(aout_sym.type & AOUT_MASK_SYMTYPE)
                {
                    case AOUT_SYMTYPE_GLOBAL:
                        if(aout_sym.type & AOUT_SYMTYPE_TEXTSEG)
                            set_value(aout_sym.symbol, text_offset + aout_sym.offset);
                        if(aout_sym.type & AOUT_SYMTYPE_DATASEG)
                            set_value(aout_sym.symbol, data_offset + aout_sym.offset);
                        if(aout_sym.type & AOUT_SYMTYPE_BSSSEG)
                            set_value(aout_sym.symbol, bss_offset + aout_sym.offset);
                        break;
                }
            }
            text_offset += _files[i].aout_header.text;
            data_offset += _files[i].aout_header.data;
            bss_offset += _files[i].aout_header.bss;
        }
    }
    text_offset = text_start;
    data_offset = data_start;
    bss_offset = bss_start;
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].name && _files[i].output_include && _files[i].aout_header.signature == AOUT_HC_IX_OBJ)
        {
            fseek(_files[i].file, _files[i].aout_offset + _files[i].aout_header.text + _files[i].aout_header.data + sizeof(aout_t), SEEK_SET);
            set_value("__textoff", text_offset);
            set_value("__dataoff", data_offset);
            set_value("__bssoff", bss_offset);
            for(s = 0; s < _files[i].aout_header.syms; s++)
            {
                fread(&aout_sym, 1, sizeof(aout_symbol_t), _files[i].file);
                switch(aout_sym.type & AOUT_MASK_SYMTYPE)
                {
                    case AOUT_SYMTYPE_REFERENCE:
                        if(aout_sym.type & AOUT_SYMTYPE_TEXTSEG)
                        {
                            fseek(_out, aout_sym.offset + text_offset - bin_offset, SEEK_SET);
                            if(aout_sym.type & AOUT_SYMTYPE_BYTE_OFFSET)
                            {
                                fread(&temp, 1, 1, _out);
                                fseek(_out, -1, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) - (aout_sym.offset + text_offset) + (int8_t)(aout_sym.type & AOUT_MASK_OFFSET) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 1, _out);
                            }
                            else if(aout_sym.type & AOUT_SYMTYPE_WORD_OFFSET)
                            {
                                fread(&temp, 1, 2, _out);
                                fseek(_out, -2, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) - (aout_sym.offset + text_offset) + (int8_t)(aout_sym.type & AOUT_MASK_OFFSET) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 2, _out);
                            }
                            else
                            {
                                fread(&temp, 1, 2, _out);
                                fseek(_out, -2, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 2, _out);
                            }
                        }
                        else if(aout_sym.type & AOUT_SYMTYPE_DATASEG)
                        {
                            fseek(_out, aout_sym.offset + _aout.text + data_offset - bin_offset, SEEK_SET);
                            if(aout_sym.type & AOUT_SYMTYPE_BYTE_OFFSET)
                            {
                                fread(&temp, 1, 1, _out);
                                fseek(_out, -1, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) - (aout_sym.offset + text_offset) + (int8_t)(aout_sym.type & AOUT_MASK_OFFSET) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 1, _out);
                            }
                            else if(aout_sym.type & AOUT_SYMTYPE_WORD_OFFSET)
                            {
                                fread(&temp, 1, 2, _out);
                                fseek(_out, -2, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) - (aout_sym.offset + text_offset) + (int8_t)(aout_sym.type & AOUT_MASK_OFFSET) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 1, _out);
                            }
                            else
                            {
                                fread(&temp, 1, 2, _out);
                                fseek(_out, -2, SEEK_CUR);
                                offset = get_value(aout_sym.symbol) + *(int16_t*)&temp;
                                fwrite(&offset, 1, 2, _out);
                            }
                        }
                        fflush(_out);
                        break;
                }
            }
            text_offset += _files[i].aout_header.text;
            data_offset += _files[i].aout_header.data;
            bss_offset += _files[i].aout_header.bss;
        }
    }
    fseek(_out, 1, SEEK_SET);
    offset = entry_sym->value - 3 - bin_offset;
    fwrite(&offset, 1,2, _out);
}

void output_pcboot(int stack_size, int include_bss)
{
    output_bin(stack_size, include_bss, 0x7c00);
}

void output_com(int stack_size, int include_bss)
{
    output_bin(stack_size, include_bss, 0x100);
}

void output_sys(int stack_size, int include_bss)
{
    output_bin(stack_size, include_bss, 0);
}
#endif
