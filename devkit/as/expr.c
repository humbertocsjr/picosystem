#include "as.h"

expr_t *expr_value()
{
    expr_t *e = 0;
    if(is_token(curr(), TOKEN_SUB))
    {
        e = clone(curr());
        e->left = clone(curr());
        e->left->token = TOKEN_INTEGER;
        e->left->value = 0;
        next();
        e->right = expr();
    }
    else if(is_token(curr(), TOKEN_ADD))
    {
        next();
        e = expr();
    }
    else if(is_token(curr(), TOKEN_INTEGER))
    {
        e = clone(curr());
        next();
    }
    else if(is_token(curr(), TOKEN_SYMBOL))
    {
        e = clone(curr());
        next();
    }
    else if(is_token(curr(), TOKEN_START_POINTER))
    {
        e = clone(curr());
        next();
    }
    else if(is_token(curr(), TOKEN_CURRENT_POINTER))
    {
        e = clone(curr());
        next();
    }
    else if(is_token(curr(), TOKEN_ADD))
    {
        e = clone(curr());
        next();
        e->right = expr();
    }
    else if(is_token(curr(), TOKEN_SUB))
    {
        e = clone(curr());
        next();
        e->right = expr();
    }
    else if(is_token(curr(), TOKEN_OPEN_PAREN))
    {
        next();
        e = expr();
        if(!is_token(curr(), TOKEN_CLOSE_PAREN))
        {
            error_at(curr(), "')' expected.");
        }
        next();
    }
    else error_at(curr(), "expression expected.");
    return e;
}

expr_t *expr_mulops()
{
    expr_t *op;
    expr_t *e = expr_value();
    while(is_token(curr(), TOKEN_MUL) || is_token(curr(), TOKEN_DIV) || is_token(curr(), TOKEN_MOD))
    {
        op = clone(curr());
        next();
        op->left = e;
        op->right = expr_value();
        e = op;
    }
    return e;
}

expr_t *expr_addops()
{
    expr_t *op;
    expr_t *e = expr_mulops();
    while(is_token(curr(), TOKEN_ADD) || is_token(curr(), TOKEN_SUB))
    {
        op = clone(curr());
        next();
        op->left = e;
        op->right = expr_mulops();
        e = op;
    }
    return e;
}

expr_t *expr()
{
    return expr_addops();
}
