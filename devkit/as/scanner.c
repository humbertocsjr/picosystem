#include "as.h"

source_t *_src = 0;
char _curr_text[TOKEN_MAX];
char _peek_text[TOKEN_MAX];
expr_t _exprs[EXPRS_MAX];

void init_scanner()
{
    int i;
    for(i = 0; i < EXPRS_MAX; i++)
    {
        _exprs[i].token = TOKEN_EMPTY;
    }
}

char read_char()
{
    _src->c = fgetc(_src->file);
    if(_src->c == EOF) _src->c = 0;
    _src->column++;
    if(_src->c == '\t') _src->column += 3;
    if(_src->c == '\r') _src->column = 0;
    if(_src->c == '\n')
    {
        _src->column = 0;
        _src->line ++;
    }
    return _src->c;
}

int is_char(char c)
{
    return _src->c == c;
}

int is_char_between(char min, char max)
{
    return min <= _src->c && _src->c <= max;
}

char get_char()
{
    return _src->c;
}

expr_t *curr()
{
    return &_src->curr;
}

expr_t *peek()
{
    return &_src->peek;
}

expr_t *next()
{
    char cat[2];
    memcpy(_curr_text, _peek_text, TOKEN_MAX);
    memset(_peek_text, 0, TOKEN_MAX);
    memcpy(&_src->curr, &_src->peek, sizeof(expr_t));
    _src->curr.text = _curr_text;
    _src->peek.text = _peek_text;
    _src->peek.value = 0;
    while(is_char(' ') || is_char('\t') || is_char('\r')  || is_char(';'))
    {
        if(is_char(';'))
        {
            while(!is_char('\n') && !is_char(0)) read_char();
        }
        else read_char();
    } 
    _src->peek.line = _src->line;
    _src->peek.column = _src->column;
    _src->peek.filename = _src->filename;
    _src->peek.token = TOKEN_EOF;
    if(is_char(0))
    {
        return curr();
    }
    if(is_char('0'))
    {
        _src->peek.token = TOKEN_INTEGER;
        read_char();
        if(is_char('x') || is_char('X'))
        {
            read_char();
            while(is_char_between('0', '9') || is_char_between('a', 'f') || is_char_between('A', 'F') || is_char('_'))
            {
                if(!is_char('_'))
                {
                    _src->peek.value <<= 4;
                    if(is_char_between('0', '9'))
                        _src->peek.value += get_char() - '0';
                    else
                        _src->peek.value += tolower(get_char()) - 'a' + 10;
                }
                read_char();
            }
        }
        else if(is_char('b') || is_char('B'))
        {
            read_char();
            while(is_char_between('0', '1') || is_char('_'))
            {
                if(!is_char('_'))
                {
                    _src->peek.value <<= 1;
                    _src->peek.value += get_char() - '0';
                }
                read_char();
            }
        }
        else if(is_char('o') || is_char('O'))
        {
            read_char();
            while(is_char_between('0', '7') || is_char('_'))
            {
                if(!is_char('_'))
                {
                    _src->peek.value <<= 3;
                    _src->peek.value += get_char() - '0';
                }
                read_char();
            }
        }
        else
        {
            while(is_char_between('0', '9') || is_char('_'))
            {
                if(!is_char('_'))
                {
                    _src->peek.value *= 10;
                    _src->peek.value += get_char() - '0';
                }
                read_char();
            }
        }
    }
    else if(is_char_between('0', '9'))
    {
        _src->peek.token = TOKEN_INTEGER;
        while(is_char_between('0', '9') || is_char('_'))
        {
            if(!is_char('_'))
            {
                _src->peek.value *= 10;
                _src->peek.value += get_char() - '0';
            }
            read_char();
        }
    }
    else if(is_char_between('0', '9') || is_char_between('a', 'z') || is_char_between('A', 'Z') || is_char('_') || is_char('.'))
    {
        _src->peek.token = TOKEN_SYMBOL;
        while(is_char_between('0', '9') || is_char_between('a', 'z') || is_char_between('A', 'Z') || is_char('_') || is_char('.'))
        {
            cat[0] = get_char();
            cat[1] = 0;
            concat(_src->peek.text, cat, TOKEN_MAX);
            read_char();
        }
    }
    else if(is_char('\''))
    {
        _src->peek.token = TOKEN_INTEGER;
        read_char();
        while(!is_char(0) && !is_char('\''))
        {
            _src->peek.value <<= 8;
            _src->peek.value |= get_char();
            read_char();
        }
        read_char();
    }
    else if(is_char('\"'))
    {
        _src->peek.token = TOKEN_STRING;
        read_char();
        while(!is_char(0) && !is_char('\"'))
        {
            cat[0] = get_char();
            cat[1] = 0;
            concat(_src->peek.text, cat, TOKEN_MAX);
            read_char();
        }
        read_char();
    }
    else if(is_char('+'))
    {
        _src->peek.token = TOKEN_ADD;
        read_char();
    }
    else if(is_char('-'))
    {
        _src->peek.token = TOKEN_SUB;
        read_char();
    }
    else if(is_char('/'))
    {
        _src->peek.token = TOKEN_DIV;
        read_char();
    }
    else if(is_char('*'))
    {
        _src->peek.token = TOKEN_MUL;
        read_char();
    }
    else if(is_char('%'))
    {
        _src->peek.token = TOKEN_MOD;
        read_char();
    }
    else if(is_char(':'))
    {
        _src->peek.token = TOKEN_LABEL;
        read_char();
    }
    else if(is_char(','))
    {
        _src->peek.token = TOKEN_COMMA;
        read_char();
    }
    else if(is_char('$'))
    {
        _src->peek.token = TOKEN_CURRENT_POINTER;
        read_char();
        if(is_char('$'))
        {
            _src->peek.token = TOKEN_START_POINTER;
            read_char();
        }
    }
    else if(is_char('['))
    {
        _src->peek.token = TOKEN_OPEN_MEMORY;
        read_char();
    }
    else if(is_char(']'))
    {
        _src->peek.token = TOKEN_CLOSE_MEMORY;
        read_char();
    }
    else if(is_char('('))
    {
        _src->peek.token = TOKEN_OPEN_PAREN;
        read_char();
    }
    else if(is_char(')'))
    {
        _src->peek.token = TOKEN_CLOSE_PAREN;
        read_char();
    }
    else if(is_char('\n'))
    {
        _src->peek.token = TOKEN_NEW_LINE;
        read_char();
    }
    else error_at(&_src->peek, "unknown token: '%c'", _src->c);
    return curr();
}

void reset_exprs()
{
    int i;
    for(i = 0; i < EXPRS_MAX; i++)
    {
        _exprs[i].token = TOKEN_EMPTY;
    }
}

expr_t *clone(expr_t *e)
{
    int i;
    if(e->token == TOKEN_EMPTY) error_at(e, "invalid token.");
    for(i = 0; i < EXPRS_MAX; i++)
    {
        if(_exprs[i].token == TOKEN_EMPTY)
        {
            memcpy(&_exprs[i], e, sizeof(expr_t));
            if(e->text) _exprs[i].text = add_name(e->text);
            return &_exprs[i];
        }
    }
    error_at(e, "expression list overflow.");
    return 0;
}

void free_tree(expr_t *e)
{
    if(!e) return;
    e->token = TOKEN_EMPTY;
    free_tree(e->left);
    free_tree(e->right);
}

void open_source(source_t *src, char *filename)
{
    _src = src;
    memset(src, 0, sizeof(source_t));
    _src->filename = add_name(filename);
    _src->file = fopen(filename, "r");
    _src->line = 1;
    if(!_src->file) error("can't open file: %s", filename);
    read_char();
}

void close_source()
{
    fclose(_src->file);
}

void set_source(source_t *src)
{
    _src = src;
}

source_t *get_source()
{
    return _src;
}

void get_pos(fpos_t *pos)
{
    fgetpos(_src->file, pos);
}

void set_pos(fpos_t *pos)
{
    fsetpos(_src->file, pos);
}

