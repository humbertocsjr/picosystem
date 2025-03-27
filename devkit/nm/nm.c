#include "../common.h"
#include "../version.h"
#include "../hosts/hosts.h"

void usage()
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico Symbol Names v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %snm [objects...]\n", get_target_prefix());
    exit(1);
}

int main(int argc, char **argv)
{
    aout_t aout;
    aout_symbol_t sym;
    FILE *file;
    int i, s;
    if(argc <= 1) usage();
    for(i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            usage();
        }
    }
    for(i = 1; i < argc; i++)
    {
        if((file = fopen(argv[i], "rb")))
        {
            fread(&aout, 1, sizeof(aout_t), file);
            if
            (
                aout.signature == AOUT_PICO_EXEC ||
                aout.signature == AOUT_PICO_OBJ
            )
            {
                fseek(file, (size_t)aout.text + (size_t)aout.data, SEEK_CUR);
                for(s = 0; s < aout.syms; s++)
                {
                    if(fread(&sym, 1, sizeof(aout_symbol_t), file) > 0)
                    {
                        printf
                        (
                            "%04x %c %s\n",
                            sym.offset,
                            sym.type & AOUT_SYMTYPE_GLOBAL ?
                            (
                                (sym.type & AOUT_SYMTYPE_TEXTSEG) ? 'T' : 
                                (sym.type & AOUT_SYMTYPE_DATASEG) ? 'D' : 
                                (sym.type & AOUT_SYMTYPE_BSSSEG) ? 'B' : '?'
                            )
                            :
                            (
                                (sym.type & AOUT_SYMTYPE_TEXTSEG) ? 't' : 
                                (sym.type & AOUT_SYMTYPE_DATASEG) ? 'd' : 
                                (sym.type & AOUT_SYMTYPE_BSSSEG) ? 'b' : '?'
                            ),
                            sym.symbol
                        );
                    }
                }
            }
            fclose(file);
        }
    }
    return 0;
}
