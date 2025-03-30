#include "../common.h"
#include "../version.h"
#include "../hosts/hosts.h"

void usage()
{
    printf("HC Pico Software Development Kit\n");
    printf("Objectify v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %sobj -o [output] -n [name] -b [binary]\n", get_target_prefix());
    exit(1);
}

int main(int argc, char **argv)
{
    aout_t aout;
    aout_symbol_t sym;
    FILE *binary = 0;
    FILE *output = 0;
    char *output_name = 0;
    char *buffer[512];
    size_t size;
    char *name = "_binary";
    int i, s;
    if(argc <= 1) usage();
    for(i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            usage();
        }
        else if(!strcmp(argv[i], "-o"))
        {
            i++;
            if(i < argc)
            {
                output = fopen(argv[i], "wb");
                if(!output)
                {
                    fprintf(stderr, "error: can't open file: %s\n", argv[i]);
                    return 1;
                }
                output_name = argv[i];
            }
        }
        else if(!strcmp(argv[i], "-b"))
        {
            i++;
            if(i < argc)
            {
                binary = fopen(argv[i], "rb");
                if(!binary)
                {
                    if(output)
                    {
                        fclose(output);
                        unlink(output_name);
                    } 
                    fprintf(stderr, "error: can't create file: %s\n", argv[i]);
                    return 1;
                }
            }
        }
        else if(!strcmp(argv[i], "-n"))
        {
            i++;
            if(i < argc)
            {
                name = argv[i];
            }
        }
        else usage();
    }
    if(!output || !binary)
    {

        if(output)
        {
            fclose(output);
            unlink(output_name);
        } 
        usage();
    } 
    fseek(binary, 0, SEEK_END);
    size = ftell(binary);
    if(size > 0xfff0) 
    {
        fprintf(stderr, "error: file size overflow.\n");
        return 1;
    }
    memset(&aout, 0, sizeof(aout_t));
    aout.signature = AOUT_PICO_OBJ;
    aout.data = size;
    aout.syms = 1;
    fwrite(&aout, 1, sizeof(aout_t), output);
    fseek(binary, 0, SEEK_SET);
    while(size = fread(buffer, 1, 512, binary))
    {
        fwrite(buffer, 1, size, output);
    }
    memset(&sym, 0, sizeof(aout_symbol_t));
    copy(sym.symbol, name, 64);
    sym.type = AOUT_SYMTYPE_DATASEG | AOUT_SYMTYPE_GLOBAL;
    fwrite(&sym, 1, sizeof(aout_symbol_t), output);
    fclose(output);
    fclose(binary);
    return 0;
}
