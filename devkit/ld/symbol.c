#include "ld.h"

symbol_t _symbols[SYMBOLS_MAX];

void init_symbol()
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        _symbols[i].name = 0;
        _symbols[i].file = 0;
    }
}

symbol_t *add_symbol(char *name, file_t *file)
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].name == 0)
        {
            _symbols[i].name = add_name(name);
            _symbols[i].file = file;
            _symbols[i].value = 0;
            return &_symbols[i];
        }
    }
    return 0;
}

long get_value(char *name)
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].name && !strcmp(name, _symbols[i].name))
        {
            return _symbols[i].value;
        }
    }
    return 0;
}

void set_value(char *name, long value)
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].name && !strcmp(name, _symbols[i].name))
        {
            _symbols[i].value = value;
            return;
        }
    }
    add_symbol(name, 0)->value = value;
}

void add_value(char *name, long value)
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].name && !strcmp(name, _symbols[i].name))
        {
            _symbols[i].value += value;
            return;
        }
    }
    add_symbol(name, 0)->value = value;
}

symbol_t *find_symbol(char *name)
{
    int i, lib, s;
    symbol_t *sym;
    aout_symbol_t aout_sym;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].name && !strcmp(_symbols[i].name, name))
        {
            return &_symbols[i];
        }
    }
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(!_symbols[i].name)
        {
            sym = &_symbols[i];
            for(lib = 0; lib < FILES_MAX; lib++)
            {
                if(_files[lib].file && _files[lib].aout_header.signature == AOUT_PICO_OBJ)
                {
                    //fseek(_files[lib].file, _files[lib].aout_offset + _files[lib].aout_header.text + _files[lib].aout_header.data + sizeof(aout_t), SEEK_SET);
                    for(s = 0; s < _files[lib].aout_header.syms; s++)
                    {
                        fseek(_files[lib].file, _files[lib].aout_offset + _files[lib].aout_header.text + _files[lib].aout_header.data + sizeof(aout_t) + (s * sizeof(aout_symbol_t)), SEEK_SET);
                        fread(&aout_sym, 1, sizeof(aout_symbol_t), _files[lib].file);
                        if((aout_sym.type & AOUT_SYMTYPE_GLOBAL) && !strncmp(aout_sym.symbol, name, 60))
                        {
                            sym->name = add_name(name);
                            sym->file = &_files[lib];
                            sym->type = aout_sym.type;
                            return sym;
                        }
                    }
                }
            }
            return 0;
        }
    }
    return 0;
}

void make_symbol_tree(symbol_t *root)
{
    int lib, s, i;
    aout_symbol_t aout_sym;
    symbol_t *sym;
    if(!root->file) return;
    root->file->output_include = 1;
    if(_verbose) printf("include: %s [%s] (TEXT+=%u DATA+=%u BSS+=%u)\n", root->name, root->file->name, root->file->aout_header.text, root->file->aout_header.data, root->file->aout_header.bss);
    
    for(s = 0; s < root->file->aout_header.syms; s++)
    {
        fseek(root->file->file, root->file->aout_offset + root->file->aout_header.text + root->file->aout_header.data + sizeof(aout_t) + (s * sizeof(aout_symbol_t)), SEEK_SET);
        fread(&aout_sym, 1, sizeof(aout_symbol_t), root->file->file);
        switch(aout_sym.type & AOUT_MASK_SYMTYPE)
        {
            case AOUT_SYMTYPE_GLOBAL:
                sym = 0;
                for(i = 0; i < SYMBOLS_MAX; i++)
                {
                    if(_symbols[i].name && !strcmp(_symbols[i].name, aout_sym.symbol))
                    {
                        sym = &_symbols[i];
                        break;
                    }
                }
                if(!sym)
                {
                    for(i = 0; i < SYMBOLS_MAX; i++)
                    {
                        sym = &_symbols[i];
                        if(!sym->name)
                        {
                            sym->name = add_name(aout_sym.symbol);
                            sym->file = root->file;
                            sym->type = aout_sym.type;
                            break;
                        }
                    }
                }
                break;
            case AOUT_SYMTYPE_REFERENCE:
                if(!(sym = find_symbol(aout_sym.symbol)))
                {
                    fprintf(stderr, "error: symbol not found: %s [%s]\n", aout_sym.symbol, root->file->name);
                    if(_out)
                    {
                        fclose(_out);
                        unlink(_out_name);
                    }
                    exit(1);
                }
                if(_verbose && sym->file)
                {
                    printf("reference: %s [%s] -> [%s]\n", aout_sym.symbol, root->file->name, sym->file->name);
                }
                if(sym->file && !sym->file->output_include)
                {
                    make_symbol_tree(sym);
                }
                break;
        }
    }
}
