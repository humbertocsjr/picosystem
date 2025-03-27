#include "ld.h"

file_t _files[FILES_MAX];
char _names[NAMES_MAX];
int _names_next = 0;
char *_dirs[DIRS_MAX];
FILE *_out = 0;
char *_out_name = 0;
aout_t _aout;
int _verbose = 0;

void init()
{
    int i;
    _out_name = "a.out";
    memset(&_aout, 0, sizeof(aout_t));
    _aout.signature = AOUT_PICO_EXEC;
    for(i = 0; i < FILES_MAX; i++)
    {
        memset(&_files[i], 0, sizeof(file_t));
    }
    for(i = 0; i < DIRS_MAX; i++)
    {
        _dirs[i] = 0;
    }
    _names_next = 0;
}

char *add_name(char *name)
{
    int len = strlen(name) + 1;
    char *ptr = &_names[_names_next];
    if((_names_next + len) >= NAMES_MAX)
    {
        fprintf(stderr, "error: name list overflow\n");
        if(_out)
        {
            fclose(_out);
            unlink(_out_name);
        }
        exit(1);
    }
    memcpy(ptr, name, len);
    _names_next += len;
    return ptr;
}

void add_file(char *name)
{
    int i;
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].file == 0)
        {
            _files[i].file = fopen(name, "rb");
            _files[i].output_include = 0;
            if(!_files[i].file)
            { 
                fprintf(stderr, "error: can't open file: %s\n", name);
                fclose(_out);
                unlink(_out_name);
                exit(1);
            }
            _files[i].aout_offset = 0;
            _files[i].name = add_name(name);
            fread(&_files[i].aout_header, 1, sizeof(aout_t), _files[i].file);
            return;
        }
    }
}

void add_library(char *name)
{
    int i;
    FILE *file = fopen(name, "rb");
    long aout_offset = 0;
    char aout_name[61];
    ar_t ar_header;
    memset(aout_name, 0, 61);
    if(_verbose) printf("library: %s\n", name);
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].file == 0)
        {
            _files[i].file = file;
            _files[i].output_include = 0;
            fseek(_files[i].file, aout_offset, SEEK_SET);
            memset(&ar_header, 0, sizeof(ar_t));
            fread(&ar_header, 1, sizeof(ar_t), _files[i].file);
            aout_offset += sizeof(ar_t);
            if(ar_header.signature == AR_AOUT)
            {
                _files[i].aout_offset = aout_offset;
                copy(aout_name, ar_header.name, 60);
                _files[i].name = add_name(aout_name);
                if(_verbose) printf("import: %s [%s]\n", _files[i].name, name);
                fread(&_files[i].aout_header, 1, sizeof(aout_t), _files[i].file);
            }
            else if(ar_header.signature == AR_TEXT)
            {

            }
            else return;
            aout_offset += ar_header.size;
        }
    }
}

void usage(int retval)
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico Linker v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %sld [-f format] [-b] [-o output] [-L dir] [-l library] [-s stack_size] [objects...]\n", get_target_prefix());
    printf("Options:\n");
    printf(" -f format       set output format (see list bellow)\n");
    printf(" -b              include bss space to output\n");
    printf(" -o output       set output file\n");
    printf(" -s stack_size   set stack size (default: 2048)\n");
    printf(" -L dir          add to library dirs list\n");
    printf(" -l library      add library (auto include 'lib' prefix)\n");
    printf("Output format:\n");
    #ifdef OUT_V7
    printf(" picosystem|pico produce Pico System Software a.out\n");
    printf(" v7|v7small      produce Seventh Edition UNIX a.out\n");
    printf(" v7tiny          tiny model output (text/data/bss in one segment)\n");
    #endif
    #ifdef OUT_BIN
    printf(" com             DOS COM executable (ORG 0x100)\n");
    printf(" sys             DOS SYS executable (ORG 0x0)\n");
    printf(" pcboot          PC Boot Sector executable (ORG 0x7c00)\n");
    #endif
    exit(retval);
}

int main(int argc, char **argv)
{
    int c, i, include_bss, ok;
    char lib_name[LIB_NAME_MAX];
    FILE *lib;
    int stack_size = 2048;
    void (*output)(int stack_size, int include_bss);
    init();
    output = 0;
    if(argc <= 1) usage(1);
    while((c = getopt(argc, argv, "hl:L:o:s:f:bv")) != -1)
    {
        switch(c)
        {
            case 'v':
                _verbose++;
                break;
            case 'b':
                include_bss = 1;
                break;
            case 'f':
                #ifdef OUT_V7
                if(!strcmp(optarg, "pico") || !strcmp(optarg, "picosystem"))
                {
                    _aout.signature = AOUT_PICO_EXEC;
                    output = output_v7;
                }
                else if(!strcmp(optarg, "v7") || !strcmp(optarg, "v7small"))
                {
                    _aout.signature = AOUT_SMALL_V7;
                    output = output_v7;
                }
                else if(!strcmp(optarg, "v7tiny"))
                {
                    _aout.signature = AOUT_TINY_V7;
                    output = output_v7;
                }
                #endif
                #ifdef OUT_BIN
                if(!strcmp(optarg, "com"))
                {
                    output = output_com;
                }
                else if(!strcmp(optarg, "sys"))
                {
                    output = output_sys;
                }
                else if(!strcmp(optarg, "pcboot"))
                {
                    output = output_pcboot;
                }
                #endif
                break;
            case 'o':
                _out_name = optarg;
                break;
            case 's':
                stack_size = atoi(optarg);
                break;
            case 'L':
                for(i = 0; i < DIRS_MAX; i++)
                {
                    if(!_dirs[i])
                    {
                        _dirs[i] = optarg;
                        break;
                    }
                }
                break;
            case 'l':
                ok = 0;
                for(i = 0; i < DIRS_MAX; i++)
                {
                    if(_dirs[i])
                        copy(lib_name, _dirs[i], LIB_NAME_MAX);
                    else
                        copy(lib_name, "", LIB_NAME_MAX);
                    concat(lib_name, optarg, LIB_NAME_MAX);
                    if((lib = fopen(lib_name, "rb")))
                    {
                        fclose(lib);
                        add_library(lib_name);
                        ok = 1;
                        break;
                    }
                    if(_dirs[i])
                    {
                        copy(lib_name, _dirs[i], LIB_NAME_MAX);
                        concat(lib_name, "/", LIB_NAME_MAX);
                    }
                    else
                        copy(lib_name, "", LIB_NAME_MAX);
                    concat(lib_name, optarg, LIB_NAME_MAX);
                    if((lib = fopen(lib_name, "rb")))
                    {
                        fclose(lib);
                        add_library(lib_name);
                        ok = 1;
                        break;
                    }
                }
                if(!ok)
                {
                    fprintf(stderr, "error: library not found: %s\n", optarg);
                    return 1;
                }
                break;
            case 'h':
                usage(0);
                return -1;
        }
    }
    if(output == 0)
    {
        usage(1);
    }
    unlink(_out_name);
    _out = fopen(_out_name, "wb+");
    if(!_out)
    {
        fprintf(stderr, "error: can't open output file: %s\n", _out_name);
        return 1;
    }
    for(i = optind; i < argc; i++)
    {
        add_file(argv[i]);
    }
    for(i = 0; i < FILES_MAX; i++)
    {
        if(_files[i].file != 0) 
        {
            _files[i].output_include = 0;
            if(_verbose) printf("object: %s\n", _files[i].name);
        }
    }
    output(stack_size, include_bss);
    fclose(_out);
    return 0;
}
