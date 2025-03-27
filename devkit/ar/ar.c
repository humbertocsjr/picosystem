#include "../common.h"
#include "../hosts/hosts.h"
#include "../version.h"

void usage(int retval)
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico Archiver v%d.%d-%s\n", VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %sar [-f archive] [-a file] [-l]\n", get_target_prefix());
    printf("Options:\n");
    printf(" -f archive      set archive file\n");
    printf(" -a file         append file to archive\n");
    printf(" -l              list all files from archive\n");
    //printf(" -x file         extract file from archive\n");
    exit(retval);
}

int main(int argc, char **argv)
{
    int c, i;
    FILE *out;
    FILE *in;
    char *out_name = "a.out";
    char *extract_name = 0;
    int add_name = 0;
    int list = 0;
    char buffer[512];
    int len;
    ar_t ar_header;
    aout_t *aout_header = (aout_t *)buffer;
    size_t size;
    if(argc <= 1) usage(1);
    while((c = getopt(argc, argv, "hf:o:al")) != -1)
    {
        switch(c)
        {
            case 'f':
            case 'o':
                out_name = optarg;
                break;
            case 'a':
                add_name = 1;
                break;
            case 'l':
                list = 1;
                break;
            case 'h':
                usage(0);
                return -1;
        }
    }
    out = fopen(out_name, add_name ? "wb+" : "rb+");
    if(!out) 
    {
        fprintf(stderr, "error: can't open file: %s\n", out_name);
        return 1;
    }
    if(add_name)
    {
        fseek(out, 0, SEEK_END);
        for(i = optind; i < argc; i++)
        {
            in = fopen(argv[i], "rb");
            fseek(in, 0, SEEK_END);
            size = ftell(in);
            fseek(in, 0, SEEK_SET);
            len = fread(buffer, 1, 512, in);
            if(aout_header->signature == AOUT_PICO_OBJ)
            {
                ar_header.signature = AR_AOUT;
            }
            else
            {
                ar_header.signature = AR_TEXT;
            }
            copy(ar_header.name, argv[i], 60);
            ar_header.size = size;
            fwrite(&ar_header, 1, sizeof(ar_t), out);
            fwrite(buffer, 1, len, out);
            while(len)
            {
                len = fread(buffer, 1, 512, in);
                if(len) fwrite(buffer, 1, len, out);
            }
        }
    }
    if(list)
    {
        fseek(out, 0, SEEK_SET);
        for(;;)
        {
            if(fread(&ar_header, 1, sizeof(ar_t), out) == 0) break;
            if(ar_header.signature == AR_AOUT)
            {
                printf("%s\n", ar_header.name);
                fseek(out, ar_header.size, SEEK_CUR);
            }
            else if(ar_header.signature == AR_TEXT)
            {
                printf("%s\n", ar_header.name);
                fseek(out, ar_header.size, SEEK_CUR);
            }
            else break;
        }
    }
    fclose(out);
    return 0;
}
