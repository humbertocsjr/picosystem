#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "../common.h"
#include "../conf.h"
#include "../version.h"
#include "../hosts/hosts.h"

#define TOKEN_SIZE 256
#define ARGS_MAX 32

#define level_t struct level
struct level
{
    level_t *prev;
    enum
    {
        LEVEL_ROOT,
        LEVEL_FUNCTION,
        LEVEL_IF,
        LEVEL_LOOP
    } type;
    int lbl_continue;
    int lbl_break;
    int lbl_exit;
    int lbl_return;
};

#define sym_t struct sym
struct sym
{
    sym_t *next;
    int offset;
    char *name;
};

#define name_t struct name
struct name
{
    name_t *next;
    char name[1];
};

#define expr_t struct expr
struct expr
{
    expr_t *left;
    expr_t *right;
    int line;
    int column;
    enum
    {
        TOK_EOF,
        TOK_SYMBOL,
        TOK_INTEGER,
        TOK_STRING,
        TOK_COMMA,
        TOK_SEMI,
        TOK_COLON,
        TOK_OPEN_INDEX,
        TOK_CLOSE_INDEX,
        TOK_OPEN_PARAMS,
        TOK_CLOSE_PARAMS,
        TOK_OPEN_BLOCK,
        TOK_CLOSE_BLOCK,
        TOK_ADD,
        TOK_SUB,
        TOK_DIV,
        TOK_MUL,
        TOK_MOD,
        TOK_SHL,
        TOK_SHR,
        TOK_INC,
        TOK_DEC,
        TOK_NOT,
        TOK_INV,
        TOK_AND,
        TOK_AND_ALSO,
        TOK_OR,
        TOK_OR_ELSE,
        TOK_XOR,
        TOK_ATTRIB,
        TOK_EQ,
        TOK_NE,
        TOK_LT,
        TOK_LE,
        TOK_GT,
        TOK_GE,
        TOK_IF,
        TOK_ELSE,
        TOK_WHILE,
        TOK_UNTIL,
        TOK_RETURN,
        TOK_BREAK,
        TOK_CONTINUE,
        TOK_AUTO,
        TOK_DO,
        TOK_CONST,
        TOK_ENUM,
        TOK_SIGNED,
        TOK_UNSIGNED,
        TOK_BYTE,
        TOK_WORD,
        TOK_FOR,
        TOK_ASM,
        TOK_BYTE_POINTER,
        TOK_WORD_POINTER,
        TOK_ADDRESS_OF,
        TOK_STRUCT
    } tok;
    int value;
    char text[1];
};

FILE *_in = 0;
char *_in_name = 0;
int _line;
int _column;
FILE *_out = 0;
char *_out_name = 0;
int _c;
int _lc;
int _prev_c;
int _inject_enter = 0;
name_t *_names = 0;
expr_t *_token = 0;
level_t *_levels = 0;
int _label = 1;
int _args = 0;
int _vars = 0;
sym_t *_globals = 0;
sym_t *_locals = 0;
sym_t *_consts = 0;

void parse();

void cg_init();
void cg_header();
void cg_footer();
void cg_global_var(char *name, int value);
void cg_global_bytearr(char *name, int size);
void cg_global_wordarr(char *name, int size);
void cg_set_acc(int value);
void cg_set_aux(int value);
void cg_push_acc();
void cg_push_aux();
void cg_pop_acc();
void cg_pop_aux();
void cg_function(char *name);
void cg_end_function();
void cg_reserve_stack(int size);
void cg_restore_stack(int size);
void cg_local_bytearr(char *name, int size);
void cg_local_wordarr(char *name, int size);
int cg_get_auto_size();
int cg_get_args_offset();
int cg_get_vars_offset();
char *cg_get_comment_start();
void cg_store_local(char *name, int offset);
void cg_store_global(char *name);
void cg_addr_local(char *name, int offset);
void cg_addr_global(char *name);
void cg_load_local(char *name, int offset);
void cg_load_global(char *name);
void cg_load_local_to_aux(char *name, int offset);
void cg_load_global_to_aux(char *name);
void cg_store_byte();
void cg_store_word();
void cg_load_byte();
void cg_load_word();
void cg_add();
void cg_call_acc();
void cg_call(char *name);
void cg_sub();
void cg_udiv();
void cg_sdiv();
void cg_mod();
void cg_umul();
void cg_smul();
void cg_eq();
void cg_ne();
void cg_ult();
void cg_slt();
void cg_ule();
void cg_sle();
void cg_ugt();
void cg_sgt();
void cg_uge();
void cg_sge();
void cg_jump(int lbl);
void cg_jump_if_true(int lbl);
void cg_jump_if_false(int lbl);
void cg_label(int lbl);
void cg_set_acc_string(char *str);
void cg_inc();
void cg_dec();
void cg_xor();
void cg_or();
void cg_and();
void cg_shl();
void cg_shr();
void cg_load_far_byte();
void cg_load_far_word();
void cg_store_far_byte();
void cg_store_far_word();

void error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if(_in_name)
    {
        if(_token)
            fprintf(stderr, "%s:%d:%d: ", _in_name, _token->line, _token->column);
        else
            fprintf(stderr, "%s:%d:%d: ", _in_name, _line, _column);
    }
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    if(_out && _out_name) 
    {
        fclose(_out);
        unlink(_out_name);
    }
    exit(1);
}

void error_at(expr_t *e, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if(_in_name)
    {
        if(e)
            fprintf(stderr, "%s:%d:%d: ", _in_name, e->line, e->column);
        else
            fprintf(stderr, "%s:%d:%d: ", _in_name, _line, _column);
    }
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    if(_out && _out_name) 
    {
        fclose(_out);
        unlink(_out_name);
    }
    exit(1);
}

void out(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(_out, fmt, args);
    fprintf(_out, "\n");
    va_end(args);
    fflush(_out);
}

void comment(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(_out, "%s", cg_get_comment_start());
    vfprintf(_out, fmt, args);
    fprintf(_out, "\n");
    va_end(args);
    fflush(_out);
}

void *alloc_obj(size_t size)
{
    void *obj = malloc(size);
    if(!obj) error("out of memory");
    return memset(obj, 0, size);
}

int new_label()
{
    return _label++;
}

void rem_level(int type)
{
    level_t *lvl = _levels;
    if(_levels->type != type) error("invalid level");
    _levels = _levels->prev;
    free(lvl);
}

level_t *new_level(int type, int lbl_continue, int lbl_break, int lbl_exit, int lbl_return)
{
    level_t *level = alloc_obj(sizeof(level_t));
    level->prev = _levels;
    _levels = level;
    level->type = type;
    level->lbl_continue = lbl_continue;
    level->lbl_break = lbl_break;
    level->lbl_exit = lbl_exit;
    level->lbl_return = lbl_return;
    return level;
}

expr_t *clone(expr_t *e)
{
    expr_t *new_e = alloc_obj(sizeof(expr_t) + strlen(e->text));
    memcpy(new_e, e, sizeof(expr_t) + strlen(e->text));
    new_e->left = 0;
    new_e->right = 0;
    return new_e;
}

void free_tree(expr_t *e)
{
    if(!e) return;
    if(e->left) free_tree(e->left);
    e->left = 0;
    if(e->right) free_tree(e->right);
    e->right = 0;
    free(e);
}

char *add_name(char *name)
{
    name_t *ptr = _names;
    while(ptr)
    {
        if(!strcmp(ptr->name, name))
        {
            return ptr->name;
        }
        ptr = ptr->next;
    }
    ptr = alloc_obj(sizeof(name_t) + strlen(name));
    ptr->next = _names;
    _names = ptr;
    strcpy(ptr->name, name);
    return ptr->name;
}

sym_t *add_global(char *name, int args)
{
    sym_t *sym = alloc_obj(sizeof(sym_t));
    sym->name = add_name(name);
    sym->offset = args;
    sym->next = _globals;
    _globals = sym;
    return sym;
}

sym_t *add_local(char *name, int offset)
{
    sym_t *sym = alloc_obj(sizeof(sym_t));
    sym->name = add_name(name);
    sym->offset = offset;
    sym->next = _locals;
    _locals = sym;
    return sym;
}

sym_t *add_const(char *name, int value)
{
    sym_t *sym = alloc_obj(sizeof(sym_t));
    sym->name = add_name(name);
    sym->offset = value;
    sym->next = _consts;
    _consts = sym;
    return sym;
}

void clear_locals()
{
    sym_t *sym;
    while(_locals)
    {
        sym = _locals->next;
        free(_locals);
        _locals = sym;
    }
}

sym_t *find_const(char *name)
{
    sym_t *sym = _consts;
    while(sym)
    {
        if(!strcmp(name, sym->name)) return sym;
        sym = sym->next;
    }
    return 0;
}

sym_t *find_local(char *name)
{
    sym_t *sym = _locals;
    while(sym)
    {
        if(!strcmp(name, sym->name)) return sym;
        sym = sym->next;
    }
    return 0;
}

sym_t *find_global(char *name)
{
    sym_t *sym = _globals;
    while(sym)
    {
        if(!strcmp(name, sym->name)) return sym;
        sym = sym->next;
    }
    return 0;
}

void get_c()
{
    _prev_c = _c;
    if(_inject_enter)
    {
        _inject_enter = 0;
        _c = '\n';
    }
    else _c = fgetc(_in);
    _column ++;
    if(_c == EOF) _c = 0;
    if(_c == 0 && _prev_c != '\n') _c = '\n';
    if(_c == '\n' && _prev_c != ' ') 
    {
        _c = ' ';
        _inject_enter = 1;
    }
    if(_c == '\n')
    {
        _line++;
        _column = 0;
    }
    if(_c == '\r')
    {
        _column = 0;
    }
    if(_c >= 'A' && _c <= 'Z') _lc = _c - 'A' + 'a';
    else _lc = _c;
}

int c_is_hex(char c)
{
    return c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';
}

int c_is_lower(char c)
{
    return c >= 'a' && c <= 'f';
}

int c_is_oct(char c)
{
    return c >= '0' && c <= '7';
}

int c_is_bin(char c)
{
    return c >= '0' && c <= '1';
}

int c_is_dec(char c)
{
    return c >= '0' && c <= '9';
}

int c_is_symbol(char c)
{
    return c >= '0' && c <= '9' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

int c_hex_to_dec(char c)
{
    if(c_is_dec(c))
    {
        return c - '0';
    }
    if(c_is_hex(c))
    {
        return c_is_lower(c) ? c - 'a' + 10 : c - 'A' + 10;
    }
    return 0;
}

void concat_c()
{
    char tmp[2];
    tmp[0] = _c;
    tmp[1] = 0;
    concat(_token->text, tmp, TOKEN_SIZE);
}

void concat_esc_c()
{
    if(_c != '\\') concat_c();
    else
    {
        get_c();
        switch(_c)
        {
            case 'n': _c = '\n'; break;
            case 'r': _c = '\r'; break;
            case 't': _c = '\t'; break;
            case 'v': _c = '\b'; break;
            case 'e': _c = 27; break;
            case 's': _c = 26; break;
            default: break;
        }
        concat_c();
    }
}

void scan()
{
    char *ptr;
    free_tree(_token);
    _token = alloc_obj(sizeof(expr_t) + TOKEN_SIZE);
    while(_c == ' ' || _c == '\n' || _c == '\t' || _c == '\r') get_c();
    _token->tok = TOK_EOF;
    _token->line = _line;
    _token->column = _column;
    if(_c == 0) return;
    if(_c == '0')
    {
        _token->tok = TOK_INTEGER;
        get_c();
        if(_c == 'x')
        {
            get_c();
            while(c_is_hex(_c))
            {
                _token->value <<= 4;
                _token->value += c_hex_to_dec(_c);
                get_c();
            }
        }
        else if(_c == 'b')
        {
            get_c();
            while(c_is_bin(_c))
            {
                _token->value <<= 1;
                _token->value += _c - '0';
                get_c();
            }
        }
        else
        {
            while(c_is_oct(_c))
            {
                _token->value <<= 3;
                _token->value += _c - '0';
                get_c();
            }
        }
    } 
    else if(c_is_dec(_c))
    {
        _token->tok = TOK_INTEGER;
        while(c_is_dec(_c))
        {
            _token->value *= 10;
            _token->value += _c - '0';
            get_c();
        }
    }
    else if(c_is_symbol(_c))
    {
        while(c_is_symbol(_c))
        {
            concat_c();
            get_c();
        }
        if(!strcmp(_token->text, "auto")) _token->tok = TOK_AUTO;
        else if(!strcmp(_token->text, "if")) _token->tok = TOK_IF;
        else if(!strcmp(_token->text, "else")) _token->tok = TOK_ELSE;
        else if(!strcmp(_token->text, "while")) _token->tok = TOK_WHILE;
        else if(!strcmp(_token->text, "until")) _token->tok = TOK_UNTIL;
        else if(!strcmp(_token->text, "do")) _token->tok = TOK_DO;
        else if(!strcmp(_token->text, "return")) _token->tok = TOK_RETURN;
        else if(!strcmp(_token->text, "break")) _token->tok = TOK_BREAK;
        else if(!strcmp(_token->text, "continue")) _token->tok = TOK_CONTINUE;
        else if(!strcmp(_token->text, "const")) _token->tok = TOK_CONST;
        else if(!strcmp(_token->text, "enum")) _token->tok = TOK_ENUM;
        else if(!strcmp(_token->text, "signed")) _token->tok = TOK_SIGNED;
        else if(!strcmp(_token->text, "unsigned")) _token->tok = TOK_UNSIGNED;
        else if(!strcmp(_token->text, "byte")) _token->tok = TOK_BYTE;
        else if(!strcmp(_token->text, "bytes")) _token->tok = TOK_BYTE;
        else if(!strcmp(_token->text, "word")) _token->tok = TOK_WORD;
        else if(!strcmp(_token->text, "words")) _token->tok = TOK_WORD;
        else if(!strcmp(_token->text, "for")) _token->tok = TOK_FOR;
        else if(!strcmp(_token->text, "asm")) _token->tok = TOK_ASM;
        else if(!strcmp(_token->text, "struct")) _token->tok = TOK_STRUCT;
        else _token->tok = TOK_SYMBOL;
        if(_token->tok != TOK_SYMBOL) strcpy(_token->text, "");
    }
    else if(_c == '"')
    {
        get_c();
        _token->tok = TOK_STRING;
        while(_c != '"')
        {
            concat_esc_c();
            get_c();
        }
        get_c();
    }
    else if(_c == '\'')
    {
        get_c();
        _token->tok = TOK_INTEGER;
        while(_c != '\'')
        {
            concat_esc_c();
            get_c();
        }
        ptr = _token->text;
        while(*ptr)
        {
            _token->value <<= 8;
            _token->value += *ptr;
            ptr++;
        }
        get_c();
        strcpy(_token->text, "");
    }
    else if(_c == '=')
    {
        _token->tok = TOK_ATTRIB;
        get_c();
        if(_c == '=')
        {
            _token->tok = TOK_EQ;
            get_c();
        }
    }
    else if(_c == '>')
    {
        _token->tok = TOK_GT;
        get_c();
        if(_c == '=')
        {
            _token->tok = TOK_GE;
            get_c();
        }
        else if(_c == '>')
        {
            _token->tok = TOK_SHR;
            get_c();
        }
    }
    else if(_c == '<')
    {
        _token->tok = TOK_LT;
        get_c();
        if(_c == '=')
        {
            _token->tok = TOK_LE;
            get_c();
        }
        else if(_c == '<')
        {
            _token->tok = TOK_SHL;
            get_c();
        }
    }
    else if(_c == '!')
    {
        _token->tok = TOK_NOT;
        get_c();
        if(_c == '=')
        {
            _token->tok = TOK_NE;
            get_c();
        }
    }
    else if(_c == '~')
    {
        _token->tok = TOK_INV;
        get_c();
    }
    else if(_c == '&')
    {
        _token->tok = TOK_AND;
        get_c();
        if(_c == '&')
        {
            _token->tok = TOK_AND_ALSO;
            get_c();
        }
    }
    else if(_c == '|')
    {
        _token->tok = TOK_OR;
        get_c();
        if(_c == '|')
        {
            _token->tok = TOK_OR_ELSE;
            get_c();
        }
    }
    else if(_c == '@')
    {
        _token->tok = TOK_BYTE_POINTER;
        get_c();
    }
    else if(_c == '^')
    {
        _token->tok = TOK_XOR;
        get_c();
    }
    else if(_c == '+')
    {
        _token->tok = TOK_ADD;
        get_c();
        if(_c == '+')
        {
            _token->tok = TOK_INC;
            get_c();
        }
    }
    else if(_c == '-')
    {
        _token->tok = TOK_SUB;
        get_c();
        if(_c == '-')
        {
            _token->tok = TOK_DEC;
            get_c();
        }
    }
    else if(_c == '%')
    {
        _token->tok = TOK_MOD;
        get_c();
    }
    else if(_c == '/')
    {
        _token->tok = TOK_DIV;
        get_c();
        if(_c == '/')
        {
            while(_c != '\n' && _c != '\r' && _c != 0)
            {
                get_c();
            }
            scan();
            return;
        }
    }
    else if(_c == '*')
    {
        _token->tok = TOK_MUL;
        get_c();
    }
    else if(_c == ';')
    {
        _token->tok = TOK_SEMI;
        get_c();
    }
    else if(_c == ':')
    {
        _token->tok = TOK_COLON;
        get_c();
    }
    else if(_c == ',')
    {
        _token->tok = TOK_COMMA;
        get_c();
    }
    else if(_c == '[')
    {
        _token->tok = TOK_OPEN_INDEX;
        get_c();
    }
    else if(_c == ']')
    {
        _token->tok = TOK_CLOSE_INDEX;
        get_c();
    }
    else if(_c == '(')
    {
        _token->tok = TOK_OPEN_PARAMS;
        get_c();
    }
    else if(_c == ')')
    {
        _token->tok = TOK_CLOSE_PARAMS;
        get_c();
    }
    else if(_c == '{')
    {
        _token->tok = TOK_OPEN_BLOCK;
        get_c();
    }
    else if(_c == '}')
    {
        _token->tok = TOK_CLOSE_BLOCK;
        get_c();
    }
    else error("unknown char: %c", _c);
}

void match(int cmp, char *expected)
{
    if(!cmp) error("%s expected.", expected);
    scan();
}

int is(int tok)
{
    return _token->tok == tok;
}

char *text()
{
    return _token->text;
}

expr_t *expr();
expr_t *expr_far();

expr_t *expr_value()
{
    expr_t *e;
    expr_t *prev_op;
    expr_t *op;
    if(is(TOK_MUL))
    {
        e = clone(_token);
        scan();
        e->tok = TOK_WORD_POINTER;
        e->right = expr_far();
        return e;
    }
    else if(is(TOK_SUB))
    {
        e = clone(_token);
        scan();
        e->left = clone(_token);
        e->left->tok = TOK_INTEGER;
        e->left->value = 0;
        e->right = expr_value();
        return e;
    }
    else if(is(TOK_ADD))
    {
        e = expr_value();
        return e;
    }
    else if(is(TOK_AND))
    {
        e = clone(_token);
        scan();
        e->tok = TOK_ADDRESS_OF;
        e->right = expr_value();
        return e;
    }
    else if(is(TOK_INC) || is(TOK_DEC) || is(TOK_BYTE_POINTER))
    {
        e = clone(_token);
        scan();
        e->right = expr_far();
        return e;
    }
    else if(is(TOK_INTEGER) || is(TOK_STRING))
    {
        e = clone(_token);
        scan();
        return e;
    }
    else if(is(TOK_SYMBOL) && find_const(_token->text))
    {
        e = clone(_token);
        e->tok = TOK_INTEGER;
        e->value = find_const(_token->text)->offset;
        scan();
        return e;
    }
    else if(is(TOK_SYMBOL))
    {
        e = clone(_token);
        scan();
        prev_op = e;
        while(is(TOK_OPEN_INDEX))
        {
            op = clone(_token);
            scan();
            op->tok = TOK_WORD;
            op->right = expr();
            if(is(TOK_BYTE))
            {
                scan();
                op->tok = TOK_BYTE;
            }
            else if(is(TOK_WORD))
            {
                scan();
                op->tok = TOK_WORD;
            }
            prev_op->left = op;
            prev_op = op;
            match(is(TOK_CLOSE_INDEX), "']'");
        }
        if(is(TOK_OPEN_PARAMS))
        {
            op = clone(_token);
            scan();
            if(!is(TOK_CLOSE_PARAMS))
            {
                free_tree(op);
                e->right = expr();
                while(is(TOK_COMMA))
                {
                    op = clone(_token);
                    scan();
                    op->right = e->right;
                    op->left = expr();
                    e->right = op;
                }
            }
            else 
            {
                e->right = op;
            }
            match(is(TOK_CLOSE_PARAMS), "')'");
        }
        if(is(TOK_INC) || is(TOK_DEC))
        {
            op = clone(_token);
            scan();
            op->left = e;
            e = op;
        }
        return e;
    }
    else if(is(TOK_OPEN_PARAMS))
    {
        match(is(TOK_OPEN_PARAMS), "'('");
        e = expr();
        match(is(TOK_CLOSE_PARAMS), "')'");
        return e;
    }
    else if(is(TOK_UNSIGNED) || is(TOK_SIGNED))
    {
        e = clone(_token);
        scan();
        match(is(TOK_OPEN_PARAMS), "'('");
        e->right = expr();
        match(is(TOK_CLOSE_PARAMS), "')'");
        return e;
    }
    error("expression expected.");
}

expr_t *expr_far()
{
    expr_t *e = expr_value();
    expr_t *op;
    while(is(TOK_COLON))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_value();
        e = op;
    }
    return e;
}

expr_t *expr_shiftops()
{
    expr_t *e = expr_far();
    expr_t *op;
    while(is(TOK_SHL) || is(TOK_SHR))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_far();
        e = op;
    }
    return e;
}

expr_t *expr_mulops()
{
    expr_t *e = expr_shiftops();
    expr_t *op;
    while(is(TOK_MUL) || is(TOK_DIV) || is(TOK_MOD))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_shiftops();
        e = op;
    }
    return e;
}

expr_t *expr_addops()
{
    expr_t *e = expr_mulops();
    expr_t *op;
    while(is(TOK_ADD) || is(TOK_SUB))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_mulops();
        e = op;
    }
    return e;
}

expr_t *expr_andops()
{
    expr_t *e = expr_addops();
    expr_t *op;
    while(is(TOK_AND))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_addops();
        e = op;
    }
    return e;
}

expr_t *expr_orops()
{
    expr_t *e = expr_andops();
    expr_t *op;
    while(is(TOK_OR) || is(TOK_XOR))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_andops();
        e = op;
    }
    return e;
}

expr_t *expr_cmpops()
{
    expr_t *e = expr_orops();
    expr_t *op;
    while(is(TOK_EQ) || is(TOK_NE) || is(TOK_LT) || is(TOK_LE) || is(TOK_GT) || is(TOK_GE))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_orops();
        e = op;
    }
    return e;
}

expr_t *expr_andalsoops()
{
    expr_t *e = expr_cmpops();
    expr_t *op;
    while(is(TOK_AND_ALSO))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_cmpops();
        e = op;
    }
    return e;
}

expr_t *expr_orelseops()
{
    expr_t *e = expr_andalsoops();
    expr_t *op;
    while(is(TOK_OR_ELSE))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_andalsoops();
        e = op;
    }
    return e;
}

expr_t *expr_attribops()
{
    expr_t *e = expr_orelseops();
    expr_t *op;
    while(is(TOK_ATTRIB))
    {
        op = clone(_token);
        scan();
        op->left = e;
        op->right = expr_orelseops();
        e = op;
    }
    return e;
}

expr_t *expr()
{
    expr_t *e = expr_attribops();
    return e;
}

expr_t *optimize(expr_t *e, int is_unsigned)
{
    if(!e) return e;
    switch(e->tok)
    {
        case TOK_SIGNED:
            e->right = optimize(e->right, 0);
            return e;
        case TOK_UNSIGNED:
            e->right = optimize(e->right, 1);
            return e;
        case TOK_INTEGER:
            return e;
        case TOK_ADD:
            if(e->left) e->left = optimize(e->left, is_unsigned);
            if(e->right) e->right = optimize(e->right, is_unsigned);
            if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
            {
                e->value = e->left->value + e->right->value;
            }
            else return e;
            break;
        case TOK_SUB:
            if(e->left) e->left = optimize(e->left, is_unsigned);
            if(e->right) e->right = optimize(e->right, is_unsigned);
            if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
            {
                e->value = e->left->value - e->right->value;
            }
            else return e;
            break;
        case TOK_MUL:
            if(e->left) e->left = optimize(e->left, is_unsigned);
            if(e->right) e->right = optimize(e->right, is_unsigned);
            if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
            {
                if(is_unsigned)
                {
                    *(unsigned *)&e->value = *(unsigned *)&e->left->value * *(unsigned *)&e->right->value;
                }
                else
                {
                    e->value = e->left->value * e->right->value;
                }
            }
            else return e;
            break;
        case TOK_DIV:
            if(e->left) e->left = optimize(e->left, is_unsigned);
            if(e->right) e->right = optimize(e->right, is_unsigned);
            if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
            {
                if(is_unsigned)
                {
                    *(unsigned *)&e->value = *(unsigned *)&e->left->value / *(unsigned *)&e->right->value;
                }
                else
                {
                    e->value = e->left->value / e->right->value;
                }
            }
            else return e;
            break;
        case TOK_MOD:
            if(e->left) e->left = optimize(e->left, is_unsigned);
            if(e->right) e->right = optimize(e->right, is_unsigned);
            if(e->left->tok == TOK_INTEGER && e->right->tok == TOK_INTEGER)
            {
                if(is_unsigned)
                {
                    *(unsigned *)&e->value = *(unsigned *)&e->left->value % *(unsigned *)&e->right->value;
                }
                else
                {
                    e->value = e->left->value % e->right->value;
                }
            }
            else return e;
            break;
        default:
            e->left = optimize(e->left, is_unsigned);
            e->right = optimize(e->right, is_unsigned); 
            return e;
    }
    free_tree(e->left);
    e->left = 0;
    free_tree(e->right);
    e->right = 0;
    e->tok = TOK_INTEGER;
    return e;
}

void parse_expr(expr_t *e, int is_unsigned);


void parse_sub_expr(expr_t *e, int is_unsigned, int can_swap)
{
    sym_t *sym;
    if(e->right->tok == TOK_SYMBOL && e->right->left == 0 && e->right->right == 0)
    {
        parse_expr(e->left, is_unsigned);
        if((sym = find_local(e->right->text)) != 0)
        {
            cg_load_local_to_aux(sym->name, sym->offset);
        }
        else if((sym = find_global(e->right->text)) != 0)
        {
            cg_load_global_to_aux(sym->name);
        }
        else error_at(e, "variable not declared: %s", e->text);
    }
    else if(e->right->tok == TOK_INTEGER)
    {
        parse_expr(e->left, is_unsigned);
        cg_set_aux(e->right->value);
    }
    else if(can_swap && e->left->tok == TOK_INTEGER)
    {
        parse_expr(e->right, is_unsigned);
        cg_set_aux(e->left->value);
    }
    else
    {
        parse_expr(e->right, is_unsigned);
        cg_push_acc();
        parse_expr(e->left, is_unsigned);
        cg_pop_aux();
    }
}

void parse_write_expr(expr_t *e, int is_unsigned)
{
    expr_t *index;
    sym_t *sym;
    int i;
    switch(e->tok)
    {
        case TOK_WORD_POINTER:
            if(e->right->tok == TOK_COLON)
            {
                cg_push_acc();
                parse_sub_expr(e->right, 1, 0);
                cg_store_far_word();
            }
            else
            {
                cg_push_acc();
                parse_expr(e->right, 0);
                cg_pop_aux();
                cg_store_word();
            }
            break;
        case TOK_BYTE_POINTER:
            if(e->right->tok == TOK_COLON)
            {
                cg_push_acc();
                parse_sub_expr(e->right, 1, 0);
                cg_store_far_byte();
            }
            else
            {
                cg_push_acc();
                parse_expr(e->right, 0);
                cg_pop_aux();
                cg_store_byte();
            }
            break;
        case TOK_SYMBOL:
            if(e->left == 0 && e->right == 0)
            {
                if((sym = find_local(e->text)) != 0)
                {
                    cg_store_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_store_global(sym->name);
                }
                else error_at(e, "variable or function not declared: %s", e->text);
            }
            else if(e->left != 0 && e->right == 0)
            {
                cg_push_acc();
                if((sym = find_local(e->text)) != 0)
                {
                    cg_load_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_load_global(sym->name);
                }
                else error_at(e, "variable or function not declared: %s", e->text);;
                index = e->left;
                while(index)
                {
                    if(index->right->tok == TOK_INTEGER)
                    {
                        cg_set_aux(index->right->value);
                    }
                    else
                    {
                        cg_push_acc();
                        parse_expr(index->right, 1);
                        cg_push_acc();
                        cg_pop_aux();
                        cg_pop_acc();
                    }
                    if(index->tok == TOK_BYTE)
                        cg_add();
                    else for(i = 0; i < cg_get_auto_size(); i++) cg_add();
                    if(!index->left)
                    {
                        cg_pop_aux();
                        if(index->tok == TOK_BYTE)
                            cg_store_byte();
                        else
                            cg_store_word();
                    }
                    else if(index->tok == TOK_BYTE)
                        cg_load_byte();
                    else
                        cg_load_word();
                    index = index->left;
                }
                
            }
            else error_at(e, "invalid expression");
            break;
        default:
            error_at(e, "invalid expression");
            break;
    }
}

void parse_addr_expr(expr_t *e, int is_unsigned)
{
    expr_t *index;
    sym_t *sym;
    int i;
    switch(e->tok)
    {
        case TOK_WORD_POINTER:
            parse_expr(e->right, 0);
            break;
        case TOK_BYTE_POINTER:
            parse_expr(e->right, 0);
            break;
        case TOK_SYMBOL:
            if(e->left == 0 && e->right == 0)
            {
                if((sym = find_local(e->text)) != 0)
                {
                    cg_addr_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_addr_global(sym->name);
                }
                else error_at(e, "variable or function not declared: %s", e->text);
            }
            else if(e->left != 0 && e->right == 0)
            {
                if((sym = find_local(e->text)) != 0)
                {
                    cg_load_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_load_global(sym->name);
                }
                else error_at(e, "variable not declared: %s", e->text);
                index = e->left;
                while(index)
                {
                    if(index->right->tok == TOK_INTEGER)
                    {
                        cg_set_aux(index->right->value);
                    }
                    else
                    {
                        cg_push_acc();
                        parse_expr(index->right, 1);
                        cg_push_acc();
                        cg_pop_aux();
                        cg_pop_acc();
                    }
                    if(index->tok == TOK_BYTE)
                        cg_add();
                    else for(i = 0; i < cg_get_auto_size(); i++) cg_add();
                    if(!index->left)
                    {
                        return;
                    }
                    else if(index->tok == TOK_BYTE)
                        cg_load_byte();
                    else
                        cg_load_word();
                    index = index->left;
                }
                
            }
            else error_at(e, "invalid expression");
            break;
        default:
            error_at(e, "invalid expression");
            break;
    }
}

void parse_expr(expr_t *e, int is_unsigned)
{
    expr_t *index;
    sym_t *sym;
    int size;
    int lbl;
    int i;
    switch(e->tok)
    {
        case TOK_AND_ALSO:  
            lbl = new_label();
            parse_expr(e->left, 0);
            cg_jump_if_false(lbl);
            parse_expr(e->right, 0);
            cg_label(lbl);
            break;
        case TOK_OR_ELSE:  
            lbl = new_label();
            parse_expr(e->left, 0);
            cg_jump_if_true(lbl);
            parse_expr(e->right, 0);
            cg_label(lbl);
            break;
        case TOK_ADDRESS_OF:
            parse_addr_expr(e->right, 0);
            break;
        case TOK_WORD_POINTER:
            if(e->right->tok == TOK_COLON)
            {
                parse_sub_expr(e->right, 1, 0);
                cg_load_far_word();
            }
            else
            {
                parse_expr(e->right, 0);
                cg_load_word();
            }
            break;
        case TOK_BYTE_POINTER:
            if(e->right->tok == TOK_COLON)
            {
                parse_sub_expr(e->right, 1, 0);
                cg_load_far_byte();
            }
            else
            {
                parse_expr(e->right, 0);
                cg_load_byte();
            }
            break;
        case TOK_INC:
            if(e->right)
            {
                parse_expr(e->right, 0);
                cg_inc();
                parse_write_expr(e->right, 0);
            }
            if(e->left)
            {
                parse_expr(e->left, 0);
                cg_push_acc();
                cg_inc();
                parse_write_expr(e->left, 0);
                cg_pop_acc();
            }
            break;
        case TOK_DEC:
            if(e->right)
            {
                parse_expr(e->right, 0);
                cg_dec();
                parse_write_expr(e->right, 0);
            }
            if(e->left)
            {
                parse_expr(e->left, 0);
                cg_push_acc();
                cg_dec();
                parse_write_expr(e->left, 0);
                cg_pop_acc();
            }
            break;
        case TOK_STRING:
            cg_set_acc_string(e->text);
            break;
        case TOK_SIGNED:
            parse_expr(e->right, 0);
            break;
        case TOK_UNSIGNED:
            parse_expr(e->right, 1);
            break;
        case TOK_INTEGER:
            cg_set_acc(e->value);
            break;
        case TOK_ATTRIB:
            parse_expr(optimize(e->right, is_unsigned), is_unsigned);
            parse_write_expr(e->left, is_unsigned);
            break;
        case TOK_AND:
            parse_sub_expr(e, is_unsigned, 1);
            cg_and();
            break;
        case TOK_XOR:
            parse_sub_expr(e, is_unsigned, 1);
            cg_xor();
            break;
        case TOK_OR:
            parse_sub_expr(e, is_unsigned, 1);
            cg_or();
            break;
        case TOK_SHL:
            parse_sub_expr(e, is_unsigned, 1);
            cg_shl();
            break;
        case TOK_SHR:
            parse_sub_expr(e, is_unsigned, 1);
            cg_shr();
            break;
        case TOK_ADD:
            parse_sub_expr(e, is_unsigned, 1);
            cg_add();
            break;
        case TOK_SUB:
            parse_sub_expr(e, is_unsigned, 0);
            cg_sub();
            break;
        case TOK_MUL:
            parse_sub_expr(e, is_unsigned, 1);
            if(is_unsigned)
                cg_umul();
            else
                cg_smul();
            break;
        case TOK_DIV:
            parse_sub_expr(e, is_unsigned, 0);
            if(is_unsigned)
                cg_udiv();
            else
                cg_sdiv();
            break;
        case TOK_MOD:
            parse_sub_expr(e, is_unsigned, 0);
            cg_mod();
            break;
        case TOK_EQ:
            parse_sub_expr(e, is_unsigned, 0);
            cg_eq();
            break;
        case TOK_NE:
            parse_sub_expr(e, is_unsigned, 0);
            cg_ne();
            break;
        case TOK_LT:
            parse_sub_expr(e, is_unsigned, 0);
            if(is_unsigned)
                cg_ult();
            else
                cg_slt();
            break;
        case TOK_LE:
            parse_sub_expr(e, is_unsigned, 0);
            if(is_unsigned)
                cg_ule();
            else
                cg_sle();
            break;
        case TOK_GT:
            parse_sub_expr(e, is_unsigned, 0);
            if(is_unsigned)
                cg_ugt();
            else
                cg_sgt();
            break;
        case TOK_GE:
            parse_sub_expr(e, is_unsigned, 0);
            if(is_unsigned)
                cg_uge();
            else
                cg_sge();
            break;
        case TOK_SYMBOL:
            if(e->left == 0 && e->right == 0)
            {
                if((sym = find_local(e->text)) != 0)
                {
                    cg_load_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_load_global(sym->name);
                }
                else error_at(e, "variable not declared: %s", e->text);
            }
            else if(e->left != 0)
            {
                if(e->right != 0)
                {
                    index = e->right;
                    size = 0;
                    comment("CALL");
                    while(index && index->tok != TOK_OPEN_PARAMS)
                    {
                        if(index->tok != TOK_COMMA)
                        {
                            comment("CALL ARGUMENT");
                            parse_expr(index, 0);
                            cg_push_acc();
                            size += cg_get_auto_size();
                            break;
                        }
                        if(index->left) 
                        {
                            comment("CALL ARGUMENT");
                            parse_expr(index->left, 0);
                            cg_push_acc();
                            size += cg_get_auto_size();
                        }
                        index = index->right;
                    }
                    comment("DO CALL");
                }
                if((sym = find_local(e->text)) != 0)
                {
                    cg_load_local(sym->name, sym->offset);
                }
                else if((sym = find_global(e->text)) != 0)
                {
                    cg_load_global(sym->name);
                }
                else error_at(e, "variable or function not declared: %s", e->text);
                index = e->left;
                while(index)
                {
                    if(index->right->tok == TOK_INTEGER)
                    {
                        cg_set_aux(index->right->value);
                    }
                    else
                    {
                        cg_push_acc();
                        parse_expr(index->right, 1);
                        cg_push_acc();
                        cg_pop_aux();
                        cg_pop_acc();
                    }
                    if(index->tok == TOK_BYTE)
                        cg_add();
                    else for(i = 0; i < cg_get_auto_size(); i++) cg_add();
                    if(index->tok == TOK_BYTE)
                        cg_load_byte();
                    else
                        cg_load_word();
                    index = index->left;
                }
                if(e->right != 0)
                {
                    cg_call_acc();
                    cg_restore_stack(size);
                    comment("END CALL");
                }
            }
            else if(e->right != 0)
            {
                if((sym = find_global(e->text)) != 0)
                {
                    index = e->right;
                    size = 0;
                    comment("CALL");
                    while(index && index->tok != TOK_OPEN_PARAMS)
                    {
                        if(index->tok != TOK_COMMA)
                        {
                            comment("CALL ARGUMENT");
                            parse_expr(index, 0);
                            cg_push_acc();
                            size += cg_get_auto_size();
                            break;
                        }
                        if(index->left) 
                        {
                            comment("CALL ARGUMENT");
                            parse_expr(index->left, 0);
                            cg_push_acc();
                            size += cg_get_auto_size();
                        }
                        index = index->right;
                    }
                    comment("DO CALL");
                    cg_call(sym->name);
                    cg_restore_stack(size);
                    comment("END CALL");
                }
                else error_at(e, "variable or function not declared: %s", e->text);
            }
            else error_at(e, "invalid expression");
            break;
        default:
            error_at(e, "invalid expression [#%d]", e->tok);
            break;
    }
}

void parse_auto()
{
    char *name; 
    int array_size;
    int value = 0;
    expr_t *e;
    scan();
    do
    {
        e = 0;
        name = add_name(text());
        value = 0;
        match(is(TOK_SYMBOL), "name");
        if(is(TOK_ATTRIB))
        {
            scan();
            e = expr();
        }
        else e = 0;
        if(is(TOK_OPEN_INDEX) && e == 0)
        {
            scan();
            e = optimize(expr(), 1);
            if(e->tok != TOK_INTEGER) error("constant expression expected.");
            if(is(TOK_BYTE))
            {
                scan();
                if(_levels->type == LEVEL_ROOT)
                {
                    comment("GLOBAL %s", name);
                    cg_global_bytearr(name, e->value);
                }
                else
                {
                    e->value += cg_get_auto_size() - 1;
                    e->value &= -cg_get_auto_size();
                    _vars -= cg_get_auto_size() + e->value;
                    comment("LOCAL %s = OFFSET %d", name, _vars);
                    cg_local_bytearr(name, e->value);
                }
            }
            else 
            {
                if(is(TOK_WORD)) scan();
                if(_levels->type == LEVEL_ROOT)
                {
                    comment("GLOBAL %s", name);
                    cg_global_wordarr(name, e->value);
                }
                else
                {
                    e->value += cg_get_auto_size() - 1;
                    e->value &= -cg_get_auto_size();
                    _vars -= cg_get_auto_size() + e->value * cg_get_auto_size();
                    comment("LOCAL %s = OFFSET %d", name, _vars);
                    cg_local_wordarr(name, e->value);
                }
            }
            match(is(TOK_CLOSE_INDEX), "']'");
        }
        else if(_levels->type == LEVEL_ROOT)
        {
            comment("GLOBAL %s", name);
            value = 0;
            if(e)
            {
                e = optimize(e, 0);
                if(e->tok != TOK_INTEGER) error("constant expression expected.");
                value = e->value;
            }
            cg_global_var(name, value);
        }
        else
        {
            _vars -= cg_get_auto_size();
            comment("LOCAL %s = OFFSET %d", name, _vars);
            if(!e)cg_set_acc(0);
            else
            {
                e = optimize(e, 0);
                parse_expr(e, 0);
            }
            cg_push_acc();
        }
        if(_levels->type == LEVEL_ROOT)
        {
            add_global(name, -1);
        }
        else
        {
            add_local(name, _vars);
        }
        if(e) free_tree(e);
        comment("END VAR %s", name);
        if(!is(TOK_COMMA)) break;
        scan();
    } while(!is(TOK_SEMI) && !is(TOK_EOF));
}

void parse_function()
{
    char *name = add_name(text());
    char *args[ARGS_MAX];
    int arg_next = 0;
    sym_t *arg;
    _args = cg_get_args_offset();
    _vars = cg_get_vars_offset();
    clear_locals();
    match(is(TOK_SYMBOL), "function name");
    match(is(TOK_OPEN_PARAMS), "'('");
    while(!is(TOK_CLOSE_PARAMS) && !is(TOK_EOF))
    {
        add_local(_token->text, _args);
        _args += cg_get_auto_size();
        match(is(TOK_SYMBOL), "argument name");
        if(!is(TOK_COMMA)) break;
        scan();
    }
    match(is(TOK_CLOSE_PARAMS), "')'");
    if(is(TOK_SEMI))
    {
        clear_locals();
        add_global(name, arg_next * cg_get_auto_size());
    }
    else
    {
        comment("FUNCTION %s", name);
        arg = _locals;
        while(arg)
        {
            comment("ARG %s = %d", arg->name, arg->offset);
            arg = arg->next;
        }
        new_level(LEVEL_FUNCTION, 0, 0, 0, new_label());
        cg_function(name);
        add_global(name, arg_next * cg_get_auto_size());
        parse();
        cg_label(_levels->lbl_return);
        cg_end_function();
        rem_level(LEVEL_FUNCTION);
        comment("END FUNCTION %s", name);
    }
}

void parse_if()
{
    expr_t *e;
    int lbl;
    scan();
    comment("IF");
    new_level(LEVEL_IF, 0, 0, new_label(), 0);
    match(is(TOK_OPEN_PARAMS), "'('");
    e = optimize(expr(), 0);
    parse_expr(e, 0);
    free_tree(e);
    cg_jump_if_false(_levels->lbl_exit);
    match(is(TOK_CLOSE_PARAMS), "')'");
    comment("IF THEN");
    parse();
    if(is(TOK_ELSE))
    {
        comment("IF ELSE");
        scan();
        lbl = new_label();
        cg_jump(lbl);
        cg_label(_levels->lbl_exit);
        _levels->lbl_exit = lbl;
        parse();
    }
    cg_label(_levels->lbl_exit);
    rem_level(LEVEL_IF);
    comment("END IF");
}

void parse_while()
{
    expr_t *e;
    int lbl;
    scan();
    comment("WHILE");
    new_level(LEVEL_LOOP, new_label(), new_label(), 0, 0);
    match(is(TOK_OPEN_PARAMS), "'('");
    cg_label(_levels->lbl_continue);
    e = optimize(expr(), 0);
    parse_expr(e, 0);
    free_tree(e);
    cg_jump_if_false(_levels->lbl_break);
    match(is(TOK_CLOSE_PARAMS), "')'");
    comment("WHILE DO");
    parse();
    cg_jump(_levels->lbl_continue);
    cg_label(_levels->lbl_break);
    rem_level(LEVEL_LOOP);
    comment("END WHILE");
}

void parse_until()
{
    expr_t *e;
    int lbl;
    scan();
    comment("UNTIL");
    new_level(LEVEL_LOOP, new_label(), new_label(), 0, 0);
    match(is(TOK_OPEN_PARAMS), "'('");
    cg_label(_levels->lbl_continue);
    e = optimize(expr(), 0);
    parse_expr(e, 0);
    free_tree(e);
    cg_jump_if_true(_levels->lbl_break);
    match(is(TOK_CLOSE_PARAMS), "')'");
    comment("UNTIL DO");
    parse();
    cg_jump(_levels->lbl_continue);
    cg_label(_levels->lbl_break);
    rem_level(LEVEL_LOOP);
    comment("END UNTIL");
}

void parse_do()
{
    expr_t *e;
    int lbl;
    scan();
    comment("DO");
    new_level(LEVEL_LOOP, new_label(), new_label(), 0, 0);
    cg_label(_levels->lbl_continue);
    parse();
    if(is(TOK_WHILE))
    {
        scan();
        match(is(TOK_OPEN_PARAMS), "'('");
        e = optimize(expr(), 0);
        parse_expr(e, 0);
        free_tree(e);
        cg_jump_if_true(_levels->lbl_continue);
        match(is(TOK_CLOSE_PARAMS), "')'");
    }
    else if(is(TOK_UNTIL))
    {
        scan();
        match(is(TOK_OPEN_PARAMS), "'('");
        e = optimize(expr(), 0);
        parse_expr(e, 0);
        free_tree(e);
        cg_jump_if_false(_levels->lbl_continue);
        match(is(TOK_CLOSE_PARAMS), "')'");
    }
    cg_label(_levels->lbl_break);
    rem_level(LEVEL_LOOP);
    comment("END DO");
}

void parse_for()
{
    expr_t *e_init = 0, *e_cmp = 0, *e_step = 0;
    scan();
    comment("FOR");
    new_level(LEVEL_LOOP, new_label(), new_label(), 0, 0);
    match(is(TOK_OPEN_PARAMS), "'('");
    e_init = optimize(expr(), 0);
    match(is(TOK_SEMI), "';'");
    e_cmp = optimize(expr(), 0);
    match(is(TOK_SEMI), "';'");
    e_step = optimize(expr(), 0);
    match(is(TOK_CLOSE_PARAMS), "')'");
    if(e_init) parse_expr(e_init, 0);
    cg_label(_levels->lbl_continue);
    if(e_cmp)
    {
        parse_expr(e_cmp, 0);
        cg_jump_if_false(_levels->lbl_break);
    }
    parse();
    if(e_step) parse_expr(e_step, 0);
    cg_jump(_levels->lbl_continue);
    cg_label(_levels->lbl_break);
    rem_level(LEVEL_LOOP);
    if(e_init) free_tree(e_init);
    if(e_cmp) free_tree(e_cmp);
    if(e_step) free_tree(e_step);
}

void parse_asm()
{
    match(is(TOK_ASM), "'asm'");
    match(is(TOK_OPEN_PARAMS), "'('");
    while(!is(TOK_CLOSE_PARAMS) && !is(TOK_EOF))
    {
        out(text());
        match(is(TOK_STRING), "Assembly code");
        if(!is(TOK_COMMA)) break;
        scan();
    }
    match(is(TOK_CLOSE_PARAMS), "')'");
}

void parse_return()
{
    level_t *lvl = _levels;
    expr_t *e;
    scan();
    while(lvl)
    {
        if(lvl->type == LEVEL_FUNCTION)
        {
            if(!is(TOK_SEMI))
            {
                e = optimize(expr(), 0);
                parse_expr(e, 0);
                free_tree(e);
            }
            cg_jump(lvl->lbl_return);
            return;
        }
        lvl = lvl->prev;
    }
    error("'return' without function");
}

void parse_break()
{
    level_t *lvl = _levels;
    expr_t *e;
    scan();
    while(lvl)
    {
        if(lvl->lbl_break)
        {
            cg_jump(lvl->lbl_break);
            return;
        }
        lvl = lvl->prev;
    }
    error("'break' without loop");
}

void parse_const()
{
    char *name;
    expr_t *e;
    scan();
    while(!is(TOK_EOF) && !is(TOK_SEMI))
    {
        name = add_name(text());
        match(is(TOK_SYMBOL), "name");
        match(is(TOK_ATTRIB), "'='");
        e = optimize(expr(), 0);
        if(e->tok != TOK_INTEGER) error_at(e, "constant integer expression");
        add_const(name, e->value);
        free_tree(e);
        if(!is(TOK_COMMA)) break;
        scan();
    }
}

void parse_struct()
{
    char *name;
    char *field;
    uint16_t last = 0;
    expr_t *e;
    scan();
    name = add_name(text());
    match(is(TOK_SYMBOL), "name");
    match(is(TOK_OPEN_BLOCK), "'{'");
    while(!is(TOK_EOF) && !is(TOK_CLOSE_BLOCK))
    {
        field = add_name(text());
        match(is(TOK_SYMBOL), "field");
        if(is(TOK_ATTRIB))
        {
            match(is(TOK_ATTRIB), "'='");
            e = optimize(expr(), 0);
            if(e->tok != TOK_INTEGER) error_at(e, "constant integer expression");
            last = e->value;
            free_tree(e);
        }
        add_const(field, last++);
        if(!is(TOK_COMMA)) break;
        scan();
    }
    match(is(TOK_CLOSE_BLOCK), "'|'");
    add_const(name, last);
}

void parse()
{
    expr_t *e;
    comment("%s:%d:%d", _in_name, _line, _column);
    if(is(TOK_AUTO))
    {
        parse_auto();
    }
    else if(is(TOK_SYMBOL) && _levels->type == LEVEL_ROOT)
    {
        parse_function();
        return;
    }
    else if(is(TOK_IF))
    {
        parse_if();
        return;
    }
    else if(is(TOK_WHILE))
    {
        parse_while();
        return;
    }
    else if(is(TOK_UNTIL))
    {
        parse_until();
        return;
    }
    else if(is(TOK_DO))
    {
        parse_do();
        return;
    }
    else if(is(TOK_FOR))
    {
        parse_for();
        return;
    }
    else if(is(TOK_ASM))
    {
        parse_asm();
    }
    else if(is(TOK_RETURN))
    {
        parse_return();
    }
    else if(is(TOK_BREAK))
    {
        parse_break();
    }
    else if(is(TOK_CONST))
    {
        parse_const();
    }
    else if(is(TOK_STRUCT))
    {
        parse_struct();
        return;
    }
    else if(is(TOK_SYMBOL) || is(TOK_INC) || is(TOK_DEC) || is(TOK_WORD_POINTER) || is(TOK_BYTE_POINTER) || is(TOK_MUL))
    {
        comment("EXPRESSION");
        e = expr();
        parse_expr(e, 0);
        free_tree(e);
        comment("END EXPRESSION");
    }
    else if(is(TOK_OPEN_BLOCK))
    {
        scan();
        while(!is(TOK_CLOSE_BLOCK) && !is(TOK_EOF))
        {
            parse();
        }
        match(is(TOK_CLOSE_BLOCK), "'}'");
        return;
    }
    else if(!is(TOK_SEMI)) error("unknown command");
    match(is(TOK_SEMI), "';'");
}

void process(char *fname)
{
    FILE *old_in = _in;
    int old_line = _line;
    
    _in = fopen(fname, "r");
    if(!_in) error("can't open file: %s", fname);
    _in_name = fname;
    _line = 1;
    get_c();
    scan();
    
    while(!is(TOK_EOF)) parse();
    
    _line = old_line;
    _in = old_in;
}

void usage(int retval)
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico B++ Compiler v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %sbpp [output] [sources ...]\n", get_target_prefix());
    exit(retval);
}

int main(int argc, char **argv)
{
    int i;
    if(argc < 2)
    {
        usage(1);
        return 1;
    }
    _out_name = argv[1];
    unlink(_out_name);
    _out = fopen(_out_name, "w");
    if(!_out) error("can't create file: %s", _out_name);
    
    new_level(LEVEL_ROOT, 0, 0, 0, 0);
    
    cg_header();
    
    for(i = 2; i < argc; i++)
    {
        process(argv[i]);
    }
    
    cg_footer();
    
    fclose(_out);
    return 0;
}
