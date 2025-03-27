#include "../common.h"
#include "../version.h"
#include "../hosts/hosts.h"

void usage()
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico Object Size v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %ssize [objects...]\n", get_target_prefix());
    exit(1);
}

int main(int argc, char **argv)
{
    aout_t aout;
    FILE *file;
    int i;
    if(argc <= 1) usage();
    for(i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            usage();
        }
    }
    printf("text\tdata\tbss\tdec\thex\tsyms\tfilename\n");
    for(i = 1; i < argc; i++)
    {
        if((file = fopen(argv[i], "rb")))
        {
            fread(&aout, 1, sizeof(aout_t), file);
            if
            (
                aout.signature == AOUT_HC_IX_EXEC ||
                aout.signature == AOUT_HC_IX_OBJ ||
                aout.signature == AOUT_TINY_V7 ||
                aout.signature == AOUT_SMALL_V7
            )
            printf
            (
                "%d\t%d\t%d\t%d\t%x\t%d\t%s\n",
                aout.text,
                aout.data,
                aout.bss,
                aout.text+aout.data+aout.bss,
                aout.text+aout.data+aout.bss,
                aout.syms,
                argv[i]
            );
            fclose(file);
        }
    }
    return 0;
}
