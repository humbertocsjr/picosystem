#include "as.h"

char _times_curr[TOKEN_MAX];
char _times_peek[TOKEN_MAX];

int32_t parse_expr(expr_t *e, int32_t offset, int can_extern)
{
    symbol_t *sym;
    if(!e) return 0;
    switch(e->token)
    {
        case TOKEN_INTEGER:
            return e->value;
        case TOKEN_CURRENT_POINTER:
            return get_offset();
        case TOKEN_START_POINTER:
            return 0;
        case TOKEN_SYMBOL:
            sym = get_symbol(e->text);
            if(sym)
            switch(sym->sym)
            {
                case SYM_LABEL:
                    if(can_extern == 3)
                    {
                        return sym->value-get_offset()+offset;
                    }
                    else if(can_extern == 2)
                    {
                        return sym->value-get_offset()+offset;
                    }
                    else if(can_extern == 1)
                    {
                        outreflocal(sym, get_offset(), offset);
                        return sym->value;
                    }
                    else error_at(e, "can't use label references here.");
                case SYM_CONST:
                    return sym->value;
                case SYM_EXTERN:
                case SYM_GLOBAL:
                    if(can_extern == 3)
                    {
                        outdispreference(sym, get_offset(), offset,2);
                        return 0;//-get_offset();
                    }
                    else if(can_extern == 2)
                    {
                        outdispreference(sym, get_offset(), offset, 1);
                        return 0;//-get_offset();
                    }
                    else if(can_extern == 1)
                    {
                        outreference(sym, get_offset(), offset);
                        return 0;
                    }
                    else error_at(e, "can't use extern references here.");
            }
            
        case TOKEN_ADD:
            return parse_expr(e->left, offset, can_extern) + parse_expr(e->right, offset, can_extern);
        case TOKEN_SUB:
            return parse_expr(e->left, offset, 0) - parse_expr(e->right, offset, 0);
        case TOKEN_MUL:
            return parse_expr(e->left, offset, 0) * parse_expr(e->right, offset, 0);
        case TOKEN_DIV:
            return parse_expr(e->left, offset, 0) / parse_expr(e->right, offset, 0);
        case TOKEN_MOD:
            return parse_expr(e->left, offset, 0) % parse_expr(e->right, offset, 0);
        default:
            break;
    }
    error_at(e, "invalid expression.");
    return 0;
}

int contains_extern(expr_t *e)
{
    symbol_t *sym;
    if(!e) return 0;
    switch(e->token)
    {
        case TOKEN_SYMBOL:
            sym = get_symbol(e->text);
            if(sym && (sym->sym == SYM_EXTERN || sym->sym == SYM_GLOBAL || sym->sym == SYM_LABEL))
                return 1;
            return 0;
        case TOKEN_ADD:
        case TOKEN_SUB:
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_MOD:
            return contains_extern(e->left) || contains_extern(e->right);
        default:
            break;
    }
    return 0;
}

void parse_param(param_t *param)
{
    if(is_register(curr()))
    {
        param->reg = parse_register();
        param->arg = arg_from_reg(param->reg);
        param->value = 0;
    }
    else if(is_token(curr(), TOKEN_OPEN_MEMORY))
    {
        next();
        param->arg = arg_from_ptr();
        param->value = 0;
        if(is_register(curr()))
        {
            param->reg = parse_pointer_register();
        }
        if(!is_token(curr(), TOKEN_CLOSE_MEMORY))
        {
            param->value = expr();
        }
        if(!is_token(curr(), TOKEN_CLOSE_MEMORY)) error_at(curr(), "']' expected.");
        next();
    }
    else
    {
        param->arg = arg_from_value();
        param->value = expr();
        if(param->value->token == TOKEN_INTEGER && param->value->value == 1)
        {
            param->arg = arg_from_1();
        }
        param->reg = 0;
    }
}

void parse_times()
{
    fpos_t pos;
    source_t *src;
    int32_t qty;
    int c, line, column;
    expr_t curr_, peek_;
    next();
    qty = parse_expr(expr(), 0, 0);
    get_pos(&pos);
    src = get_source();
    c = src->c;
    line = src->line;
    column = src->column;
    memcpy(&curr_, &src->curr, sizeof(expr_t));
    memcpy(&peek_, &src->peek, sizeof(expr_t));
    memcpy(_times_curr, src->curr.text, TOKEN_MAX);
    memcpy(_times_peek, src->peek.text, TOKEN_MAX);
    if(qty == 0)
    {
        while(!is_token(curr(), TOKEN_EOF) && !is_token(curr(), TOKEN_NEW_LINE))
        {
            next();
        }
        return;
    }
    else if(qty < 0)
    {
        error_at(curr(), "invalid negative times value.");
    }
    while(qty--)
    {
        set_pos(&pos);
        src->c = c;
        src->line = line;
        src->column = column;
        memcpy(&src->curr, &curr_, sizeof(expr_t));
        memcpy(&src->peek, &peek_, sizeof(expr_t));
        memcpy(src->curr.text, _times_curr, TOKEN_MAX);
        memcpy(src->peek.text, _times_peek, TOKEN_MAX);
        parse();
    }
}

void parse()
{
    opcode_t *op = _opcodes;
    param_t params[PARAMS_MAX];
    int params_qty = 0;
    int found = 0;
    int i;
    int32_t value;
    symbol_t *sym;
    char mnemonic[MNEMONIC_MAX];
    reset_exprs();
    if(is_token(curr(), TOKEN_NEW_LINE))
    {
        next();
        return;
    }
    if(is(curr(), "section"))
    {
        next();
        if(is(curr(), ".text"))
        {
            set_segment(SEGMENT_TEXT);
            next();
        }
        else if(is(curr(), ".data"))
        {
            set_segment(SEGMENT_DATA);
            next();
        }
        else if(is(curr(), ".bss"))
        {
            set_segment(SEGMENT_BSS);
            next();
        }
    }
    else if(is(curr(), "extern"))
    {
        next();
        if(!is_token(curr(), TOKEN_SYMBOL)) error_at(curr(), "symbol name expected.");
        add_symbol(SYM_EXTERN, curr()->text, 0);
        next();
    }
    else if(is(curr(), "global"))
    {
        next();
        if(!is_token(curr(), TOKEN_SYMBOL)) error_at(curr(), "symbol name expected.");
        sym = get_symbol(curr()->text);
        outpublic(add_symbol(SYM_GLOBAL, curr()->text, sym != 0 ? sym->value : 0));
        next();
    }
    else if(is(curr(), "resb"))
    {
        next();
        reserve(parse_expr(expr(), 0, 0));
    }
    else if(is(curr(), "resw"))
    {
        next();
        reserve(parse_expr(expr(), 0, 0) * 2);
    }
    else if(is(curr(), "resw"))
    {
        next();
        reserve(parse_expr(expr(), 0, 0) * 4);
    }
    else if(is(curr(), "db"))
    {
        next();
        while(!is_token(curr(), TOKEN_EOF) && !is_token(curr(), TOKEN_NEW_LINE))
        {
            if(is_token(curr(), TOKEN_STRING))
            {
                for(i = 0; i < strlen(curr()->text); i++)
                {
                    outb(curr()->text[i]);
                }
                next();
            }
            else
            {
                outb(parse_expr(expr(), 0, 0));
            }
            if(!is_token(curr(), TOKEN_COMMA)) break;
            next();
        }
    }
    else if(is(curr(), "dw"))
    {
        next();
        while(!is_token(curr(), TOKEN_EOF) && !is_token(curr(), TOKEN_NEW_LINE))
        {
            outw(parse_expr(expr(), 0, 1));
            if(!is_token(curr(), TOKEN_COMMA)) break;
            next();
        }
    }
    else if(is(curr(), "dd"))
    {
        next();
        while(!is_token(curr(), TOKEN_EOF) && !is_token(curr(), TOKEN_NEW_LINE))
        {
            value = parse_expr(expr(), 0, 1);
            outw(value & 0xffff);
            outw((value >> 16) & 0xffff);
            if(!is_token(curr(), TOKEN_COMMA)) break;
            next();
        }
    }
    else if(is(curr(), "times"))
    {
        parse_times();
        return;
    }
    else if(is_token(curr(), TOKEN_SYMBOL) && is_token(peek(), TOKEN_LABEL))
    {
        sym = add_symbol(SYM_LABEL, curr()->text, get_offset());
        next();
        next();
        if(is(curr(), "equ"))
        {
            next();
            sym->value = parse_expr(expr(), 0, 0);
            sym->sym = SYM_CONST;
        }
        return;
    }
    else if(is_token(curr(), TOKEN_SYMBOL) && is(peek(), "equ"))
    {
        sym = add_symbol(SYM_CONST, curr()->text, 0);
        next();
        next();
        sym->value = parse_expr(expr(), 0, 0);
    }
    else
    {
        if(!is_token(curr(), TOKEN_SYMBOL)) error_at(curr(), "mnemonic expected.");
        strncpy(mnemonic, curr()->text, MNEMONIC_MAX-1);
        next();
        if(is(curr(), "far"))
        {
            concat(mnemonic, " far", MNEMONIC_MAX);
            next();
        }
        else if(is(curr(), "near"))
        {
            concat(mnemonic, " near", MNEMONIC_MAX);
            next();
        }
        else if(is(curr(), "short"))
        {
            concat(mnemonic, " short", MNEMONIC_MAX);
            next();
        }
        else if(is(curr(), "word"))
        {
            concat(mnemonic, " word", MNEMONIC_MAX);
            next();
        }
        else if(is(curr(), "byte"))
        {
            concat(mnemonic, " byte", MNEMONIC_MAX);
            next();
        }
        for(i = 0; i < PARAMS_MAX; i++)
        {
            params[i].reg = 0;
            params[i].arg = 0;
            params[i].value = 0;
        }
        for(i = 0; i < PARAMS_MAX; i++)
        {
            if(is_token(curr(), TOKEN_NEW_LINE) || is_token(curr(), TOKEN_EOF)) break;
            parse_param(&params[i]);
            if(!is_token(curr(), TOKEN_COMMA) && !is_token(curr(), TOKEN_LABEL)) break;
            next();
        }
        while(op->mnemonic)
        {
            if(!strcmp(op->mnemonic, mnemonic) || (op->alt_mnemonic && !strcmp(op->alt_mnemonic, mnemonic)))
            {
                found = 1;
                for(i = 0; i < (PARAMS_MAX); i++)
                {
                    if(op->params[i] == 0 && params[i].arg == op->params[i]) break;
                    found = (params[i].arg & op->params[i]);
                    if(!found) break;
                }
                if(found)
                { 
                    op->emit(op, params, params_qty);
                    for(i = 0; i < (PARAMS_MAX); i++)
                    {
                        free_tree(params[i].value);
                        params[i].value = 0;
                    }
                    break;
                }
            }
            op++;
        }
        if(!found) error_at(curr(), "unknown command: '%s'", mnemonic);
    }
    if(is_token(curr(), TOKEN_EOF)) return;
    if(!is_token(curr(), TOKEN_NEW_LINE)) error_at(curr(), "new line expected.");
    next();
}
