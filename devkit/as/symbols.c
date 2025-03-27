#include "as.h"

symbol_t _symbols[SYMBOLS_MAX];
symbol_t *_last_root_symbol = 0;
char _temp_name[TOKEN_MAX];

void init_symbol()
{
    int i;
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        _symbols[i].sym = SYM_EMPTY;
    }
    strcpy(_temp_name, "");
}

symbol_t *add_symbol(int sym, char *name, int32_t value)
{
    int i;
    int root_name = 1;
    if(name[0] == '.' && _last_root_symbol)
    {
        copy(_temp_name, _last_root_symbol->name, TOKEN_MAX);
        concat(_temp_name, name, TOKEN_MAX);
        name = _temp_name;
        root_name = 0;
    }
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].sym != SYM_EMPTY && !strcmp(_symbols[i].name, name))
        {
            _symbols[i].value = value;
            _symbols[i].seg = get_segment();
            if(root_name) _last_root_symbol = &_symbols[i];
            return &_symbols[i];
        }
    }
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].sym == SYM_EMPTY)
        {
            _symbols[i].sym = sym;
            _symbols[i].name = add_name(name);
            _symbols[i].value = value;
            _symbols[i].seg = get_segment();
            if(root_name) _last_root_symbol = &_symbols[i];
            inc_changes();
            return &_symbols[i];
        }
    }
    error_at(curr(), "symbol list overflow: %s", name);
    return 0;
}

void set_symbol(char *name, int32_t value)
{
    int i;
    int root_name = 1;
    if(name[0] == '.' && _last_root_symbol)
    {
        strncpy(_temp_name, _last_root_symbol->name, TOKEN_MAX);
        concat(_temp_name, name, TOKEN_MAX);
        name = _temp_name;
        root_name = 0;
    }
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].sym != SYM_EMPTY && !strcmp(_symbols[i].name, name))
        {
            if(_symbols[i].value != value) inc_changes();
            _symbols[i].value = value;
            _symbols[i].seg = get_segment();
            if(root_name) _last_root_symbol = &_symbols[i];
            return;
        }
    }
    add_symbol(SYM_CONST, name, value);
}

symbol_t *get_symbol(char *name)
{
    int i;
    if(name[0] == '.' && _last_root_symbol)
    {
        strncpy(_temp_name, _last_root_symbol->name, TOKEN_MAX);
        concat(_temp_name, name, TOKEN_MAX);
        name = _temp_name;
    }
    for(i = 0; i < SYMBOLS_MAX; i++)
    {
        if(_symbols[i].sym != SYM_EMPTY && !strcmp(_symbols[i].name, name))
        {
            return &_symbols[i];
        }
    }
    if(get_pass() == 2)
    {
        add_symbol(SYM_EXTERN, name, 0);
    }
    return 0;
}
