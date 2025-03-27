#include "as.h"


void error_at(expr_t *e, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s:%d:%d: error: ", e->filename, e->line, e->column);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    remove_out();
    exit(1);
}

void error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    remove_out();
    exit(1);
}
