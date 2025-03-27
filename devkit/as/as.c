#include "as.h"

aout_t _header;
int _changed = 0;
int _pass = 0;

int get_pass()
{
    return _pass;
}

void inc_changes()
{
    _changed ++;
    if(_changed == 0)_changed++;
}

void process_file(char *filename)
{
    source_t src;
    source_t *old_src;
    old_src = get_source();
    open_source(&src, filename);
    next();
    next();
    while(!is_token(curr(), TOKEN_EOF)) parse();
    close_source();
    set_source(old_src);
}

void reset()
{
    memset(&_header, 0, sizeof(aout_t));
    _header.signature = AOUT_PICO_OBJ;
}

void usage(int retval)
{
    printf("HC Pico Software Development Kit\n");
    printf("Pico %s Assembler v%d.%d-%s\n", get_target_name(), VERSION, REVISION, EDITION);
    printf("Copyright (c) 2025, Humberto Costa dos Santos Junior\n\n");
    printf("Usage: %sas [-o output] [objects...]\n", get_target_prefix());
    printf("Options:\n");
    printf(" -o output       set output file\n");
    exit(retval);
}

int main(int argc, char **argv)
{
    int c, i;
    char * out_name = "a.out";
    init_scanner();
    init_symbol();
    if(argc <= 1) usage(1);
    while((c = getopt(argc, argv, "ho:")) != -1)
    {
        switch(c)
        {
            case 'o':
                out_name = optarg;
                break;
            case 'h':
                usage(0);
                return -1;
        }
    }
    open_out(out_name);
    do
    {
        reset();
        _changed = 0;
        _pass++;
        for(i = optind; i < argc; i++)
        {
            process_file(argv[i]);
        }
    } while(_changed);
    set_output_segment(SEGMENT_HEADER);
    outraw(&_header, sizeof(aout_t));
    set_output_segment(SEGMENT_TEXT);
    reset();
    for(i = optind; i < argc; i++)
    {
        process_file(argv[i]);
    }
    set_output_segment(SEGMENT_DATA);
    reset();
    for(i = optind; i < argc; i++)
    {
        process_file(argv[i]);
    }
    set_output_segment(SEGMENT_SYMBOLS);
    reset();
    for(i = optind; i < argc; i++)
    {
        process_file(argv[i]);
    }
    close_out();
    return 0;
}
