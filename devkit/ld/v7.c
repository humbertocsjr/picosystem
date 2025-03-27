#include "ld.h"

#ifdef OUT_V7

void output_v7(int stack_size, int include_bss)
{
    int i, len, total, s, offset;
    long text_offset, data_offset, bss_offset;
    long text_start, data_start, bss_start;
    int16_t temp;
    symbol_t *entry_sym;
    char buffer[BUFFER_MAX];
    char *overflow_msg = 0;
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
    set_value("__end", stack_size);
    make_symbol_tree(entry_sym);
    for(i = 0; i < FILES_MAX; i++)
    {
        overflow_msg = 0;
        if(_files[i].name && _files[i].output_include)
        {
            add_value("__etext", _files[i].aout_header.text);
            add_value("__edata", _files[i].aout_header.data);
            add_value("__end", _files[i].aout_header.bss);
            if(get_value("__etext") >= 0xfff0)
            {
                overflow_msg = "text segment size overflow.";
            }
            if((get_value("__edata") + get_value("__end")) >= 0xffe0)
            {
                overflow_msg = "data/bss segment size overflow.";
            }
        }
        if(overflow_msg)
        {
            fprintf(stderr, "%s\n", overflow_msg);
            fclose(_out);
            unlink(_out_name);
            exit(1);
        }
    }
    _aout.text = (get_value("__etext") + 15) & 0xfff0;
    _aout.data = (get_value("__edata") + 15) & 0xfff0;
    _aout.bss = (get_value("__end") + 15) & 0xfff0;
    set_value("__segoff", _aout.text >> 4);
    if(_aout.signature == AOUT_TINY_V7)
    {
        text_start = 0;
        set_value("__etext", _aout.text);
        data_start = _aout.text;
        set_value("__edata", _aout.data + _aout.text);
        bss_start = _aout.data + _aout.text;
        set_value("__end", _aout.bss + _aout.data + _aout.text);
    }
    else 
    {
        text_start = 0;
        set_value("__etext", _aout.text);
        data_start = 0;
        set_value("__edata", _aout.data);
        bss_start = _aout.data;
        set_value("__end", _aout.bss + _aout.data);
    }
    fwrite(&_aout, 1, sizeof(aout_t), _out);
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
    fseek(_out, sizeof(aout_t) + _aout.text, SEEK_SET);
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
        fseek(_out, sizeof(aout_t) + _aout.text + _aout.data + _aout.bss, SEEK_SET);
    else
        fseek(_out, sizeof(aout_t) + _aout.text + _aout.data, SEEK_SET);
    fputc('H', _out);
    fputc('C', _out);
    fputc('D', _out);
    fputc('E', _out);
    fputc('V', _out);
    fputc('K', _out);
    fputc('I', _out);
    fputc('T', _out);
    fputc(VERSION, _out);
    fputc(REVISION, _out);
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
                            fseek(_out, sizeof(aout_t) + aout_sym.offset + text_offset, SEEK_SET);
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
                            fseek(_out, sizeof(aout_t) + aout_sym.offset + _aout.text + data_offset, SEEK_SET);
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
                        break;
                }
            }
            text_offset += _files[i].aout_header.text;
            data_offset += _files[i].aout_header.data;
            bss_offset += _files[i].aout_header.bss;
        }
    }
    _aout.entry = entry_sym->value;
    fseek(_out, 0, SEEK_SET);
    fwrite(&_aout, 1, sizeof(aout_t), _out);
}

#endif
