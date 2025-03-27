#include "as.h"

int is(expr_t *e, char *symbol)
{
    char *s1, *s2;
    if(e->token != TOKEN_SYMBOL) return 0;
    if(!e->text || !symbol) return 0;
    s1 = e->text;
    s2 = symbol;
    while((tolower(*s1) == tolower(*s2)))
    {
        if(*s1 == 0) return 1;
        s1++;
        s2++;
    }
    return 0;
}

int is_token(expr_t *e, token_t token)
{
    return e->token == token;
}
