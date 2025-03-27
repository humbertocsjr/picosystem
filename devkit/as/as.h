#ifndef AS_H
#define AS_H


#include "../common.h"
#include "../version.h"
#include "../hosts/hosts.h"
#include <stdint.h>

#define TOKEN_MAX 256
#define EXPRS_MAX 128
#define NAMES_MAX 8192
#define PARAMS_MAX 10
#define OPCODES_MAX 10
#define SYMBOLS_MAX 1024
#define MNEMONIC_MAX 10

typedef enum token_t
{
    TOKEN_EMPTY,
    TOKEN_EOF,
    TOKEN_NEW_LINE,
    TOKEN_SYMBOL,
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_COMMA,
    TOKEN_LABEL,
    TOKEN_OPEN_MEMORY,
    TOKEN_CLOSE_MEMORY,
    TOKEN_START_POINTER,
    TOKEN_CURRENT_POINTER,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN
} token_t;

typedef struct expr_t
{
    char *filename;
    int line;
    int column;
    struct expr_t *left;
    struct expr_t *right;
    token_t token;
    int32_t value;
    char *text;
} expr_t;

typedef struct source_t
{
    FILE *file;
    char *filename;
    int line;
    int column;
    int c;
    expr_t curr;
    expr_t peek;
} source_t;

typedef struct param_t
{
    int arg;
    int reg;
    expr_t *value;
} param_t;

typedef struct opcode_t
{
    char *mnemonic;
    char *alt_mnemonic;
    int params[PARAMS_MAX];
    int opcodes[OPCODES_MAX];
    void (*emit)(struct opcode_t *op, param_t *params, int params_qty);
} opcode_t;

enum sym_t
{
    SYM_EMPTY,
    SYM_LABEL,
    SYM_GLOBAL,
    SYM_CONST,
    SYM_EXTERN
};

typedef struct symbol_t
{
    int sym;
    char *name;
    int32_t value;
    segment_t seg;
} symbol_t;

// as.c
extern aout_t _header;
int get_pass();
void inc_changes();

// names.c
char *add_name(char *name);

// token.c
int is(expr_t *e, char *symbol);
int is_token(expr_t *e, token_t token);

// scanner.c
void init_scanner();
expr_t * curr();
expr_t * peek();
expr_t * next();
void reset_exprs();
expr_t *clone(expr_t *e);
void free_tree(expr_t *e);
void open_source(source_t *src, char *filename);
void close_source();
void set_source(source_t *src);
source_t *get_source();
void get_pos(fpos_t *pos);
void set_pos(fpos_t *pos);

// error.c
void error_at(expr_t *e, char *fmt, ...);
void error(char *fmt, ...);

// out.c
void open_out(char *filename);
void remove_out();
void close_out();
void set_segment(segment_t seg);
segment_t get_segment();
void set_output_segment(segment_t seg);
int can_output();
void reserve(int size);
void outb(int8_t value);
void outw(int16_t value);
void outd(int32_t value);
void outraw(void *data, int size);
void outreference(symbol_t *sym, int32_t pointer, int32_t offset);
void outdispreference(symbol_t *sym, int32_t pointer, int32_t offset, int size);
void outreflocal(symbol_t *sym, int32_t pointer, int32_t offset);
void outdispreflocal(symbol_t *sym, int32_t pointer, int32_t offset, int size);
void outpublic(symbol_t *sym);
int get_offset();

// symbols.c
void init_symbol();
symbol_t *add_symbol(int sym, char *name, int32_t value);
void set_symbol(char *name, int32_t value);
symbol_t *get_symbol(char *name);

// parser.c
int32_t parse_expr(expr_t *e, int32_t offset, int can_extern);
int contains_extern(expr_t *e);
void parse();

// expr.c
expr_t *expr();

// target
extern opcode_t _opcodes[];
int is_register(expr_t *e);
int parse_register();
int parse_pointer_register();
char *get_target();
int arg_from_reg(int reg);
int arg_from_value();
int arg_from_ptr();
int arg_from_1();

#endif
