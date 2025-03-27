#ifndef LD_H
#define LD_H

#include "../conf.h"
#include "../common.h"
#include "../version.h"
#include "../hosts/hosts.h"

#define LIB_NAME_MAX 128
#define FILES_MAX 128
#define NAMES_MAX 10240
#define SYMBOLS_MAX 1024
#define DIRS_MAX 16
#define BUFFER_MAX 128

typedef struct file_t
{
    FILE *file;
    char *name;
    long aout_offset;
    aout_t aout_header;
    int output_include;
    long output_offset;
} file_t;

typedef struct symbol_t
{
    file_t *file;
    uint16_t type;
    char *name;
    long value;
} symbol_t;

// ld.c
extern FILE *_out;
extern char *_out_name;
extern aout_t _aout;
extern file_t _files[FILES_MAX];
extern int _verbose;
char *add_name(char *name);

// symbol.c
void init_symbol();
symbol_t *add_symbol(char *name, file_t *file);
long get_value(char *name);
void set_value(char *name, long value);
void add_value(char *name, long value);
void make_symbol_tree(symbol_t *root);
symbol_t *find_symbol(char *name);

// aout16.c
void output_v7(int stack_size, int include_bss);

// bin.c
void output_com(int stack_size, int include_bss);
void output_sys(int stack_size, int include_bss);
void output_pcboot(int stack_size, int include_bss);

// target
char get_bin_jump();

#endif
