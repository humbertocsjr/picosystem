#include "../as.h"

enum
{
    ARG_NONE = 0,
    ARG_8BIT = 0x01,
    ARG_16BIT = 0x02,
    ARG_SEG = 0x04,
    ARG_PTR = 0x08,
    ARG_VALUE = 0x10,
    ARG_AL = 0x20,
    ARG_AX = 0x40,
    ARG_DX = 0x80,
    ARG_CL = 0x100,
    ARG_1 = 0x200
};

enum
{
    REGTYPE_16BIT = 0x100,
    REGTYPE_8BIT = 0x200,
    REGTYPE_SEG = 0x400,
    REGTYPE_PTR = 0x800,
    MASK_VALUE = 0xff,
    REG_AX = 0x100,
    REG_CX = 0x101,
    REG_DX = 0x102,
    REG_BX = 0x103,
    REG_SP = 0x104,
    REG_BP = 0x105,
    REG_SI = 0x106,
    REG_DI = 0x107,
    REG_AL = 0x200,
    REG_CL = 0x201,
    REG_DL = 0x202,
    REG_BL = 0x203,
    REG_AH = 0x204,
    REG_CH = 0x205,
    REG_DH = 0x206,
    REG_BH = 0x207,
    SEG_ES = 0x400,
    SEG_CS = 0x401,
    SEG_SS = 0x402,
    SEG_DS = 0x403,
    PTR_BX_SI = 0x800,
    PTR_BX_DI = 0x801,
    PTR_BP_SI = 0x802,
    PTR_BP_DI = 0x803,
    PTR_SI = 0x804,
    PTR_DI = 0x805,
    PTR_BP = 0x806,
    PTR_BX = 0x807,
};

int is_register(expr_t *e)
{
    if(e->token != TOKEN_SYMBOL) return 0;
    if(is(e, "ax")) return REG_AX;
    if(is(e, "bx")) return REG_BX;
    if(is(e, "cx")) return REG_CX;
    if(is(e, "dx")) return REG_DX;
    if(is(e, "sp")) return REG_SP;
    if(is(e, "bp")) return REG_BP;
    if(is(e, "si")) return REG_SI;
    if(is(e, "di")) return REG_DI;
    if(is(e, "al")) return REG_AL;
    if(is(e, "bl")) return REG_BL;
    if(is(e, "cl")) return REG_CL;
    if(is(e, "dl")) return REG_DL;
    if(is(e, "ah")) return REG_AH;
    if(is(e, "bh")) return REG_BH;
    if(is(e, "ch")) return REG_CH;
    if(is(e, "dh")) return REG_DH;
    if(is(e, "es")) return SEG_ES;
    if(is(e, "cs")) return SEG_CS;
    if(is(e, "ss")) return SEG_SS;
    if(is(e, "ds")) return SEG_DS;
    return 0;
}

int parse_register()
{
    int reg = is_register(curr());
    if(!reg) error_at(curr(), "register expected.");
    next();
    return reg;
}

int parse_pointer_register()
{
    int reg = parse_register();
    switch (reg) 
    {
        case REG_BP:
            reg = PTR_BP;
            if(is_token(curr(), TOKEN_ADD) && is_register(peek()))
            {
                switch(is_register(peek()))
                {
                    case REG_SI:
                        reg = PTR_BP_SI;
                        next();
                        next();
                        break;
                    case REG_DI:
                        reg = PTR_BP_DI;
                        next();
                        next();
                        break;
                    default:
                        error_at(peek(), "invalid register.");
                }
            }
            break;
        case REG_BX:
            reg = PTR_BX;
            if(is_token(curr(), TOKEN_ADD) && is_register(peek()))
            {
                switch(is_register(peek()))
                {
                    case REG_SI:
                        reg = PTR_BX_SI;
                        next();
                        next();
                        break;
                    case REG_DI:
                        reg = PTR_BX_DI;
                        next();
                        next();
                        break;
                    default:
                        error_at(peek(), "invalid register.");
                }
            }
            break;
        case REG_SI:
            reg = PTR_SI;
            if(is_token(curr(), TOKEN_ADD) && is_register(peek()))
            {
                switch(is_register(peek()))
                {
                    case REG_BP:
                        reg = PTR_BP_SI;
                        next();
                        next();
                        break;
                    case REG_BX:
                        reg = PTR_BX_SI;
                        next();
                        next();
                        break;
                    default:
                        error_at(peek(), "invalid register.");
                }
            }
            break;
        case REG_DI:
            reg = PTR_DI;
            if(is_token(curr(), TOKEN_ADD) && is_register(peek()))
            {
                switch(is_register(peek()))
                {
                    case REG_BP:
                        reg = PTR_BP_DI;
                        next();
                        next();
                        break;
                    case REG_BX:
                        reg = PTR_BX_DI;
                        next();
                        next();
                        break;
                    default:
                        error_at(peek(), "invalid register.");
                }
            }
            break;
        default:
            error_at(curr(), "pointer register expected.");
    }
    return reg;
}

void emit_simple(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
}

void emit_2_simple(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1]);
}

void emit_reg(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0] | (params[0].reg & MASK_VALUE));
}

void emit_none_reg(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0] | (params[1].reg & MASK_VALUE));
}

void emit_reg3shl(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0] | ((params[0].reg & MASK_VALUE)<<3));
}

void emit_disp16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outw(parse_expr(params[0].value, -2, 3));
}

void emit_disp8(opcode_t *op, param_t *params, int params_qty)
{
    int32_t value;
    outb(op->opcodes[0]);
    value = parse_expr(params[0].value, -1, 2);
    if(value < INT8_MIN || INT8_MAX < value)
    {
        error_at(params[0].value, "invalid offset: %d", value);
    }
    outb(value);
}

void emit_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outw(parse_expr(params[0].value, -3, 1));
}

void emit_value16_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outw(parse_expr(params[0].value, -3, 1));
    outw(parse_expr(params[1].value, -5, 1));
}

void emit_value16_value16_inv(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outw(parse_expr(params[1].value, -3, 1));
    outw(parse_expr(params[0].value, -5, 1));
}

void emit_value8(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(parse_expr(params[0].value, -2, 1));
}

void emit_none_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outw(parse_expr(params[1].value, -3, 1));
}

void emit_none_value8(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(parse_expr(params[1].value, -2, 1));
}

void emit_reg_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0] | (params[0].reg & MASK_VALUE));
    outw(parse_expr(params[1].value, 0, 0));
}

void emit_reg_value8(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0] | (params[0].reg & MASK_VALUE));
    outb(parse_expr(params[1].value, 0, 0));
}

void emit_mrm_reg_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[0].reg & MASK_VALUE) );
    outw(parse_expr(params[1].value, -3, 1));
}

void emit_mrm_reg_value8(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[0].reg & MASK_VALUE) );
    outb(parse_expr(params[1].value, -2, 1));
}

void emit_mrm_reg(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[0].reg & MASK_VALUE));
}

void emit_mrm_reg_reg(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[0].reg & MASK_VALUE) | ((params[1].reg & MASK_VALUE) << 3));
}

void emit_mrm_reg_reg_inv(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[1].reg & MASK_VALUE) | ((params[0].reg & MASK_VALUE) << 3));
}

void emit_mrm_ptr_value16(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1]  | (params[0].reg == 0 ? 0x06 : (0x80 | (params[0].reg & MASK_VALUE))));
    outw(parse_expr(params[0].value, -3, 1));
    outw(parse_expr(params[1].value, -5, 1));
}

void emit_mrm_ptr_value8(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1]  | (params[0].reg == 0 ? 0x06 : (0x80 | (params[0].reg & MASK_VALUE))));
    outw(parse_expr(params[0].value, -3, 1));
    outb(parse_expr(params[1].value, -4, 0));
}

void emit_mrm_reg_ptr(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | ((params[0].reg & MASK_VALUE)<<3) | (params[1].reg == 0 ? 0x06 : (0x80 | (params[1].reg & MASK_VALUE))));
    outw(parse_expr(params[1].value, -3, 1));
}

void emit_mrm_ptr_reg(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | ((params[1].reg & MASK_VALUE)<<3) | (params[0].reg == 0 ? 0x06 : (0x80 | (params[0].reg & MASK_VALUE))));
    outw(parse_expr(params[0].value, -3, 1));
}

void emit_mrm_ptr(opcode_t *op, param_t *params, int params_qty)
{
    outb(op->opcodes[0]);
    outb(op->opcodes[1] | (params[0].reg == 0 ? 0x06 : (0x80 | (params[0].reg & MASK_VALUE))));
    outw(parse_expr(params[0].value, -3, 1));
}

char *get_target()
{
    return "i86";
}

int arg_from_reg(int reg)
{
    if(reg == REG_DX) return ARG_DX | ARG_16BIT;
    if(reg == REG_AX) return ARG_AX | ARG_16BIT;
    if(reg == REG_AL) return ARG_AL | ARG_8BIT;
    if(reg == REG_CL) return ARG_CL | ARG_8BIT;
    if(reg & REGTYPE_16BIT) return ARG_16BIT;
    if(reg & REGTYPE_8BIT) return ARG_8BIT;
    if(reg & REGTYPE_SEG) return ARG_SEG;
    error_at(curr(), "register type not implemented.");
    return 0;
}

int arg_from_value()
{
    return ARG_VALUE;
}

int arg_from_1()
{
    return ARG_VALUE | ARG_1;
}

int arg_from_ptr()
{
    return ARG_PTR;
}

opcode_t _opcodes[] = 
{
    {"nop", 0, {ARG_NONE}, {0x90}, emit_simple},
    {"mov", "mov word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0xc7, 0xc0}, emit_mrm_reg_value16},
    {"mov", "mov byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0xc6, 0xc0}, emit_mrm_reg_value8},
    {"mov", "mov word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x89, 0xc0}, emit_mrm_reg_reg},
    {"mov", "mov byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x88, 0xc0}, emit_mrm_reg_reg},
    {"mov word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0xc7, 0x00}, emit_mrm_ptr_value16},
    {"mov byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0xc6, 0x00}, emit_mrm_ptr_value8},
    {"mov", "mov word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x8b, 0x00}, emit_mrm_reg_ptr},
    {"mov", "mov byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x8a, 0x00}, emit_mrm_reg_ptr},
    {"mov", "mov word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x89, 0x00}, emit_mrm_ptr_reg},
    {"mov", "mov byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x88, 0x00}, emit_mrm_ptr_reg},
    {"mov", 0, {ARG_SEG, ARG_16BIT, ARG_NONE}, {0x8e, 0xc0}, emit_mrm_reg_reg_inv},
    {"mov", 0, {ARG_16BIT, ARG_SEG, ARG_NONE}, {0x8c, 0xc0}, emit_mrm_reg_reg},
    {"mov", 0, {ARG_SEG, ARG_PTR, ARG_NONE}, {0x8e, 0x00}, emit_mrm_reg_ptr},
    {"mov", 0, {ARG_PTR, ARG_SEG, ARG_NONE}, {0x8c, 0x00}, emit_mrm_ptr_reg},
    {"push", "push word", {ARG_16BIT, ARG_NONE}, {0x50}, emit_reg},
    {"push", "push word", {ARG_SEG, ARG_NONE}, {0x06}, emit_reg3shl},
    {"push word", 0, {ARG_PTR, ARG_NONE}, {0xff, 0x30}, emit_mrm_ptr},
    {"pop", "pop word", {ARG_16BIT, ARG_NONE}, {0x58}, emit_reg},
    {"pop", "pop word", {ARG_SEG, ARG_NONE}, {0x07}, emit_reg3shl},
    {"pop word", 0, {ARG_PTR, ARG_NONE}, {0x8f, 0x00}, emit_mrm_ptr},
    {"xchg", "xchg word", {ARG_AX, ARG_16BIT, ARG_NONE}, {0x90, 0x00}, emit_none_reg},
    {"xchg", "xchg word", {ARG_16BIT, ARG_AX, ARG_NONE}, {0x90, 0x00}, emit_reg},
    {"xchg", "xchg word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x87, 0xc0}, emit_mrm_reg_reg},
    {"xchg", "xchg word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x87, 0x00}, emit_mrm_ptr_reg},
    {"xchg", "xchg word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x87, 0x00}, emit_mrm_reg_ptr},
    {"xchg", "xchg byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x86, 0xc0}, emit_mrm_reg_reg},
    {"xchg", "xchg byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x86, 0x00}, emit_mrm_ptr_reg},
    {"xchg", "xchg byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x86, 0x00}, emit_mrm_reg_ptr},
    {"in", "in word", {ARG_AX, ARG_DX, ARG_NONE}, {0xed}, emit_simple},
    {"in", "in byte", {ARG_AL, ARG_DX, ARG_NONE}, {0xec}, emit_simple},
    {"in", "in word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0xe5}, emit_none_value8},
    {"in", "in byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0xe4}, emit_none_value8},
    {"out", "out word", {ARG_DX, ARG_AX, ARG_NONE}, {0xef}, emit_simple},
    {"out", "out byte", {ARG_DX, ARG_AL, ARG_NONE}, {0xee}, emit_simple},
    {"out", "out word", {ARG_VALUE, ARG_AX, ARG_NONE}, {0xe7}, emit_value8},
    {"out", "out byte", {ARG_VALUE, ARG_AL, ARG_NONE}, {0xe6}, emit_value8},
    {"xlat", "xlatb", {ARG_NONE}, {0xd7}, emit_simple},
    {"lea", 0, {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x8d}, emit_mrm_reg_ptr},
    {"lds", 0, {ARG_16BIT, ARG_PTR, ARG_NONE}, {0xc5}, emit_mrm_reg_ptr},
    {"les", 0, {ARG_16BIT, ARG_PTR, ARG_NONE}, {0xc4}, emit_mrm_reg_ptr},
    {"lahf", 0, {ARG_NONE}, {0x9f}, emit_simple},
    {"sahf", 0, {ARG_NONE}, {0x9e}, emit_simple},
    {"pushf", 0, {ARG_NONE}, {0x9c}, emit_simple},
    {"popf", 0, {ARG_NONE}, {0x9d}, emit_simple},
    {"add", "add word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x05}, emit_none_value16},
    {"add", "add byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x04}, emit_none_value8},
    {"add", "add word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x01, 0xc0}, emit_mrm_reg_reg},
    {"add", "add byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x00, 0xc0}, emit_mrm_reg_reg},
    {"add word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x00}, emit_mrm_ptr_value16},
    {"add byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x00}, emit_mrm_ptr_value8},
    {"add", "add word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x03, 0x00}, emit_mrm_reg_ptr},
    {"add", "add byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x02, 0x00}, emit_mrm_reg_ptr},
    {"add", "add word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x01, 0x00}, emit_mrm_ptr_reg},
    {"add", "add byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x00, 0x00}, emit_mrm_ptr_reg},
    {"add", "add word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xc0}, emit_mrm_reg_value16},
    {"add", "add byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xc0}, emit_mrm_reg_value8},
    {"adc", "adc word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x15}, emit_none_value16},
    {"adc", "adc byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x14}, emit_none_value8},
    {"adc", "adc word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x11, 0xc0}, emit_mrm_reg_reg},
    {"adc", "adc byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x10, 0xc0}, emit_mrm_reg_reg},
    {"adc word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x10}, emit_mrm_ptr_value16},
    {"adc byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x10}, emit_mrm_ptr_value8},
    {"adc", "adc word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x13, 0x00}, emit_mrm_reg_ptr},
    {"adc", "adc byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x12, 0x00}, emit_mrm_reg_ptr},
    {"adc", "adc word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x11, 0x00}, emit_mrm_ptr_reg},
    {"adc", "adc byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x10, 0x00}, emit_mrm_ptr_reg},
    {"adc", "adc word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xd0}, emit_mrm_reg_value16},
    {"adc", "adc byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xd0}, emit_mrm_reg_value8},
    {"inc", "inc word", {ARG_16BIT, ARG_NONE}, {0x40}, emit_reg},
    {"inc", "inc byte", {ARG_8BIT, ARG_NONE}, {0xfe, 0xc0}, emit_mrm_reg},
    {"inc word", 0, {ARG_PTR, ARG_NONE}, {0xff, 0x00}, emit_mrm_ptr},
    {"inc byte", 0, {ARG_PTR, ARG_NONE}, {0xfe, 0x00}, emit_mrm_ptr},
    {"aaa", 0, {ARG_NONE}, {0x37}, emit_simple},
    {"baa", 0, {ARG_NONE}, {0x27}, emit_simple},
    {"sub", "sub word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x2d}, emit_none_value16},
    {"sub", "sub byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x2c}, emit_none_value8},
    {"sub", "sub word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x29, 0xc0}, emit_mrm_reg_reg},
    {"sub", "sub byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x28, 0xc0}, emit_mrm_reg_reg},
    {"sub word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x28}, emit_mrm_ptr_value16},
    {"sub byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x28}, emit_mrm_ptr_value8},
    {"sub", "sub word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x2b, 0x00}, emit_mrm_reg_ptr},
    {"sub", "sub byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x2a, 0x00}, emit_mrm_reg_ptr},
    {"sub", "sub word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x29, 0x00}, emit_mrm_ptr_reg},
    {"sub", "sub byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x28, 0x00}, emit_mrm_ptr_reg},
    {"sub", "sub word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xe8}, emit_mrm_reg_value16},
    {"sub", "sub byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xe8}, emit_mrm_reg_value8},
    {"sbb", "sbb word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x1d}, emit_none_value16},
    {"sbb", "sbb byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x1c}, emit_none_value8},
    {"sbb", "sbb word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x19, 0xc0}, emit_mrm_reg_reg},
    {"sbb", "sbb byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x18, 0xc0}, emit_mrm_reg_reg},
    {"sbb word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x18}, emit_mrm_ptr_value16},
    {"sbb byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x18}, emit_mrm_ptr_value8},
    {"sbb", "sbb word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x1b, 0x00}, emit_mrm_reg_ptr},
    {"sbb", "sbb byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x1a, 0x00}, emit_mrm_reg_ptr},
    {"sbb", "sbb word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x19, 0x00}, emit_mrm_ptr_reg},
    {"sbb", "sbb byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x18, 0x00}, emit_mrm_ptr_reg},
    {"sbb", "sbb word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xd8}, emit_mrm_reg_value16},
    {"sbb", "sbb byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xd8}, emit_mrm_reg_value8},
    {"dec", "dec word", {ARG_16BIT, ARG_NONE}, {0x48}, emit_reg},
    {"dec", "dec byte", {ARG_8BIT, ARG_NONE}, {0xfe, 0xc8}, emit_mrm_reg},
    {"dec word", 0, {ARG_PTR, ARG_NONE}, {0xff, 0x08}, emit_mrm_ptr},
    {"dec byte", 0, {ARG_PTR, ARG_NONE}, {0xfe, 0x08}, emit_mrm_ptr},
    {"neg", "neg byte", {ARG_16BIT, ARG_NONE}, {0xf7, 0xd8}, emit_mrm_reg},
    {"neg", "neg byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xd8}, emit_mrm_reg},
    {"neg word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x18}, emit_mrm_ptr},
    {"neg byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x18}, emit_mrm_ptr},
    {"cmp", "cmp word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x3d}, emit_none_value16},
    {"cmp", "cmp byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x3c}, emit_none_value8},
    {"cmp", "cmp word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xf8}, emit_mrm_reg_value16},
    {"cmp", "cmp byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xf8}, emit_mrm_reg_value8},
    {"cmp", "cmp word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x39, 0xc0}, emit_mrm_reg_reg},
    {"cmp", "cmp byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x38, 0xc0}, emit_mrm_reg_reg},
    {"cmp word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x38}, emit_mrm_ptr_value16},
    {"cmp byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x38}, emit_mrm_ptr_value8},
    {"cmp", "cmp word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x3b, 0x00}, emit_mrm_reg_ptr},
    {"cmp", "cmp byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x3a, 0x00}, emit_mrm_reg_ptr},
    {"cmp", "cmp word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x39, 0x00}, emit_mrm_ptr_reg},
    {"cmp", "cmp byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x38, 0x00}, emit_mrm_ptr_reg},
    {"aas", 0, {ARG_NONE}, {0x3f}, emit_simple},
    {"das", 0, {ARG_NONE}, {0x2f}, emit_simple},
    {"mul", "mul word", {ARG_16BIT, ARG_NONE}, {0xf7, 0xe0}, emit_mrm_reg},
    {"mul", "mul byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xe0}, emit_mrm_reg},
    {"mul word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x20}, emit_mrm_ptr},
    {"mul byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x20}, emit_mrm_ptr},
    {"imul", "imul word", {ARG_16BIT, ARG_NONE}, {0xf7, 0xe8}, emit_mrm_reg},
    {"imul", "imul byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xe8}, emit_mrm_reg},
    {"imul word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x28}, emit_mrm_ptr},
    {"imul byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x28}, emit_mrm_ptr},
    {"aam", 0, {ARG_NONE}, {0xd4, 0x0a}, emit_2_simple},
    {"div", "div word", {ARG_16BIT, ARG_NONE}, {0xf7, 0xf0}, emit_mrm_reg},
    {"div", "div byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xf0}, emit_mrm_reg},
    {"div word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x30}, emit_mrm_ptr},
    {"div byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x30}, emit_mrm_ptr},
    {"idiv", "idiv word", {ARG_16BIT, ARG_NONE}, {0xf7, 0xf8}, emit_mrm_reg},
    {"idiv", "idiv byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xf8}, emit_mrm_reg},
    {"idiv word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x38}, emit_mrm_ptr},
    {"idiv byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x38}, emit_mrm_ptr},
    {"aad", 0, {ARG_NONE}, {0xd5, 0x0a}, emit_2_simple},
    {"cbw", 0, {ARG_NONE}, {0x98}, emit_simple},
    {"cwd", 0, {ARG_NONE}, {0x99}, emit_simple},
    {"not", "not word", {ARG_16BIT, ARG_NONE}, {0xf7, 0xc8}, emit_mrm_reg},
    {"not", "not byte", {ARG_8BIT, ARG_NONE}, {0xf6, 0xc8}, emit_mrm_reg},
    {"not word", 0, {ARG_PTR, ARG_NONE}, {0xf7, 0x08}, emit_mrm_ptr},
    {"not byte", 0, {ARG_PTR, ARG_NONE}, {0xf6, 0x08}, emit_mrm_ptr},
    {"sal", "sal word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xe0}, emit_mrm_reg},
    {"sal", "sal byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xe0}, emit_mrm_reg},
    {"sal word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x20}, emit_mrm_ptr},
    {"sal byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x20}, emit_mrm_ptr},
    {"sal", "sal word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xe0}, emit_mrm_reg},
    {"sal", "sal byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xe0}, emit_mrm_reg},
    {"sal word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x20}, emit_mrm_ptr},
    {"sal byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x20}, emit_mrm_ptr},
    {"shl", "shl word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xe0}, emit_mrm_reg},
    {"shl", "shl byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xe0}, emit_mrm_reg},
    {"shl word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x20}, emit_mrm_ptr},
    {"shl byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x20}, emit_mrm_ptr},
    {"shl", "shl word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xe0}, emit_mrm_reg},
    {"shl", "shl byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xe0}, emit_mrm_reg},
    {"shl word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x20}, emit_mrm_ptr},
    {"shl byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x20}, emit_mrm_ptr},
    {"shr", "shr word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xe8}, emit_mrm_reg},
    {"shr", "shr byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xe8}, emit_mrm_reg},
    {"shr word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x28}, emit_mrm_ptr},
    {"shr byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x28}, emit_mrm_ptr},
    {"shr", "shr word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xe8}, emit_mrm_reg},
    {"shr", "shr byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xe8}, emit_mrm_reg},
    {"shr word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x28}, emit_mrm_ptr},
    {"shr byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x28}, emit_mrm_ptr},
    {"sar", "sar word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xf8}, emit_mrm_reg},
    {"sar", "sar byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xf8}, emit_mrm_reg},
    {"sar word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x38}, emit_mrm_ptr},
    {"sar byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x38}, emit_mrm_ptr},
    {"sar", "sar word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xf8}, emit_mrm_reg},
    {"sar", "sar byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xf8}, emit_mrm_reg},
    {"sar word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x38}, emit_mrm_ptr},
    {"sar byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x38}, emit_mrm_ptr},
    {"rol", "rol word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xc0}, emit_mrm_reg},
    {"rol", "rol byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xc0}, emit_mrm_reg},
    {"rol word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x00}, emit_mrm_ptr},
    {"rol byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x00}, emit_mrm_ptr},
    {"rol", "rol word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xc0}, emit_mrm_reg},
    {"rol", "rol byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xc0}, emit_mrm_reg},
    {"rol word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x00}, emit_mrm_ptr},
    {"rol byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x00}, emit_mrm_ptr},
    {"ror", "ror word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xc8}, emit_mrm_reg},
    {"ror", "ror byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xc8}, emit_mrm_reg},
    {"ror word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x08}, emit_mrm_ptr},
    {"ror byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x08}, emit_mrm_ptr},
    {"ror", "ror word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xc8}, emit_mrm_reg},
    {"ror", "ror byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xc8}, emit_mrm_reg},
    {"ror word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x08}, emit_mrm_ptr},
    {"ror byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x08}, emit_mrm_ptr},
    {"rcl", "rcl word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xd0}, emit_mrm_reg},
    {"rcl", "rcl byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xd0}, emit_mrm_reg},
    {"rcl word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x10}, emit_mrm_ptr},
    {"rcl byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x10}, emit_mrm_ptr},
    {"rcl", "rcl word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xd0}, emit_mrm_reg},
    {"rcl", "rcl byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xd0}, emit_mrm_reg},
    {"rcl word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x10}, emit_mrm_ptr},
    {"rcl byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x10}, emit_mrm_ptr},
    {"rcr", "rcr word", {ARG_16BIT, ARG_1, ARG_NONE}, {0xd1, 0xd8}, emit_mrm_reg},
    {"rcr", "rcr byte", {ARG_8BIT, ARG_1, ARG_NONE}, {0xd0, 0xd8}, emit_mrm_reg},
    {"rcr word", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd1, 0x18}, emit_mrm_ptr},
    {"rcr byte", 0, {ARG_PTR, ARG_1, ARG_NONE}, {0xd0, 0x18}, emit_mrm_ptr},
    {"rcr", "rcr word", {ARG_16BIT, ARG_CL, ARG_NONE}, {0xd3, 0xd8}, emit_mrm_reg},
    {"rcr", "rcr byte", {ARG_8BIT, ARG_CL, ARG_NONE}, {0xd2, 0xd8}, emit_mrm_reg},
    {"rcr word", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd3, 0x18}, emit_mrm_ptr},
    {"rcr byte", 0, {ARG_PTR, ARG_CL, ARG_NONE}, {0xd2, 0x18}, emit_mrm_ptr},
    {"and", "and word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x25}, emit_none_value16},
    {"and", "and byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x24}, emit_none_value8},
    {"and", "and word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x21, 0xc0}, emit_mrm_reg_reg},
    {"and", "and byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x20, 0xc0}, emit_mrm_reg_reg},
    {"and word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x20}, emit_mrm_ptr_value16},
    {"and byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x20}, emit_mrm_ptr_value8},
    {"and", "and word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x23, 0x00}, emit_mrm_reg_ptr},
    {"and", "and byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x22, 0x00}, emit_mrm_reg_ptr},
    {"and", "and word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x21, 0x00}, emit_mrm_ptr_reg},
    {"and", "and byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x20, 0x00}, emit_mrm_ptr_reg},
    {"and", "and word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xe0}, emit_mrm_reg_value16},
    {"and", "and byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xe0}, emit_mrm_reg_value8},
    {"test", "test word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0xa9}, emit_none_value16},
    {"test", "test byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0xa8}, emit_none_value8},
    {"test", "test word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x85, 0xc0}, emit_mrm_reg_reg},
    {"test", "test byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x84, 0xc0}, emit_mrm_reg_reg},
    {"test word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0xf7, 0x00}, emit_mrm_ptr_value16},
    {"test byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0xf6, 0x00}, emit_mrm_ptr_value8},
    {"test", "test word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x85, 0x00}, emit_mrm_reg_ptr},
    {"test", "test byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x84, 0x00}, emit_mrm_reg_ptr},
    {"test", "test word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x85, 0x00}, emit_mrm_ptr_reg},
    {"test", "test byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x84, 0x00}, emit_mrm_ptr_reg},
    {"test", "test word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0xf7, 0xc0}, emit_mrm_reg_value16},
    {"test", "test byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0xf6, 0xc0}, emit_mrm_reg_value8},
    {"or", "or word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x0d}, emit_none_value16},
    {"or", "or byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x0c}, emit_none_value8},
    {"or", "or word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x09, 0xc0}, emit_mrm_reg_reg},
    {"or", "or byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x08, 0xc0}, emit_mrm_reg_reg},
    {"or word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x08}, emit_mrm_ptr_value16},
    {"or byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x08}, emit_mrm_ptr_value8},
    {"or", "or word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x0b, 0x00}, emit_mrm_reg_ptr},
    {"or", "or byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x0a, 0x00}, emit_mrm_reg_ptr},
    {"or", "or word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x09, 0x00}, emit_mrm_ptr_reg},
    {"or", "or byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x08, 0x00}, emit_mrm_ptr_reg},
    {"or", "or word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xc8}, emit_mrm_reg_value16},
    {"or", "or byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xc8}, emit_mrm_reg_value8},
    {"xor", "xor word", {ARG_AX, ARG_VALUE, ARG_NONE}, {0x35}, emit_none_value16},
    {"xor", "xor byte", {ARG_AL, ARG_VALUE, ARG_NONE}, {0x34}, emit_none_value8},
    {"xor", "xor word", {ARG_16BIT, ARG_16BIT, ARG_NONE}, {0x31, 0xc0}, emit_mrm_reg_reg},
    {"xor", "xor byte", {ARG_8BIT, ARG_8BIT, ARG_NONE}, {0x30, 0xc0}, emit_mrm_reg_reg},
    {"xor word", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x81, 0x30}, emit_mrm_ptr_value16},
    {"xor byte", 0, {ARG_PTR, ARG_VALUE, ARG_NONE}, {0x80, 0x30}, emit_mrm_ptr_value8},
    {"xor", "xor word", {ARG_16BIT, ARG_PTR, ARG_NONE}, {0x33, 0x00}, emit_mrm_reg_ptr},
    {"xor", "xor byte", {ARG_8BIT, ARG_PTR, ARG_NONE}, {0x32, 0x00}, emit_mrm_reg_ptr},
    {"xor", "xor word", {ARG_PTR, ARG_16BIT, ARG_NONE}, {0x31, 0x00}, emit_mrm_ptr_reg},
    {"xor", "xor byte", {ARG_PTR, ARG_8BIT, ARG_NONE}, {0x30, 0x00}, emit_mrm_ptr_reg},
    {"xor", "xor word", {ARG_16BIT, ARG_VALUE, ARG_NONE}, {0x81, 0xf0}, emit_mrm_reg_value16},
    {"xor", "xor byte", {ARG_8BIT, ARG_VALUE, ARG_NONE}, {0x80, 0xf0}, emit_mrm_reg_value8},
    {"repnz", "repne", {ARG_NONE}, {0xf2}, emit_simple},
    {"repz", "repe", {ARG_NONE}, {0xf3}, emit_simple},
    {"rep", 0, {ARG_NONE}, {0xf3}, emit_simple},
    {"movsw", 0, {ARG_NONE}, {0xa5}, emit_simple},
    {"movsb", 0, {ARG_NONE}, {0xa4}, emit_simple},
    {"cmpsw", 0, {ARG_NONE}, {0xa7}, emit_simple},
    {"cmpsb", 0, {ARG_NONE}, {0xa6}, emit_simple},
    {"scasw", 0, {ARG_NONE}, {0xaf}, emit_simple},
    {"scasb", 0, {ARG_NONE}, {0xae}, emit_simple},
    {"lodsw", 0, {ARG_NONE}, {0xad}, emit_simple},
    {"lodsb", 0, {ARG_NONE}, {0xac}, emit_simple},
    {"stosw", 0, {ARG_NONE}, {0xab}, emit_simple},
    {"stosb", 0, {ARG_NONE}, {0xaa}, emit_simple},
    {"call", "call near", {ARG_VALUE, ARG_NONE}, {0xe8}, emit_disp16},
    {"call", "call near", {ARG_PTR, ARG_NONE}, {0xff, 0x10}, emit_mrm_ptr},
    {"call", "call near", {ARG_16BIT, ARG_NONE}, {0xff, 0xd0}, emit_mrm_reg},
    {"call", "call far", {ARG_VALUE, ARG_VALUE, ARG_NONE}, {0x9a}, emit_value16_value16_inv},
    {"call far", 0, {ARG_PTR, ARG_NONE}, {0xff, 0x18}, emit_mrm_ptr},
    {"jmp", "jmp near", {ARG_VALUE, ARG_NONE}, {0xe9}, emit_disp16},
    {"jmp short", 0, {ARG_VALUE, ARG_NONE}, {0xeb}, emit_disp8},
    {"jmp", "jmp near", {ARG_PTR, ARG_NONE}, {0xff, 0x20}, emit_mrm_ptr},
    {"jmp", "jmp near", {ARG_16BIT, ARG_NONE}, {0xff, 0xe0}, emit_mrm_reg},
    {"jmp", "jmp far", {ARG_VALUE, ARG_VALUE, ARG_NONE}, {0xea}, emit_value16_value16_inv},
    {"jmp far", 0, {ARG_PTR, ARG_NONE}, {0xff, 0x28}, emit_mrm_ptr},
    {"ret", "retn", {ARG_NONE}, {0xc3}, emit_simple},
    {"ret", "retn", {ARG_VALUE, ARG_NONE}, {0xc2}, emit_value16},
    {"reti", "retf", {ARG_NONE}, {0xcb}, emit_simple},
    {"reti", "retf", {ARG_VALUE, ARG_NONE}, {0xca}, emit_value16},
    {"jc", 0, {ARG_VALUE, ARG_NONE}, {0x72}, emit_disp8},
    {"jnc", 0, {ARG_VALUE, ARG_NONE}, {0x73}, emit_disp8},
    {"jz", "je", {ARG_VALUE, ARG_NONE}, {0x74}, emit_disp8},
    {"jl", "jnge", {ARG_VALUE, ARG_NONE}, {0x7c}, emit_disp8},
    {"jle", "jng", {ARG_VALUE, ARG_NONE}, {0x7e}, emit_disp8},
    {"jb", "jnae", {ARG_VALUE, ARG_NONE}, {0x72}, emit_disp8},
    {"jbe", "jna", {ARG_VALUE, ARG_NONE}, {0x76}, emit_disp8},
    {"jp", "jpe", {ARG_VALUE, ARG_NONE}, {0x7a}, emit_disp8},
    {"jo", 0, {ARG_VALUE, ARG_NONE}, {0x70}, emit_disp8},
    {"js", 0, {ARG_VALUE, ARG_NONE}, {0x78}, emit_disp8},
    {"jne", "jnz", {ARG_VALUE, ARG_NONE}, {0x75}, emit_disp8},
    {"jnl", "jge", {ARG_VALUE, ARG_NONE}, {0x7d}, emit_disp8},
    {"jnle", "jg", {ARG_VALUE, ARG_NONE}, {0x7f}, emit_disp8},
    {"jnb", "jae", {ARG_VALUE, ARG_NONE}, {0x73}, emit_disp8},
    {"jnbe", "ja", {ARG_VALUE, ARG_NONE}, {0x77}, emit_disp8},
    {"jnp", "jpo", {ARG_VALUE, ARG_NONE}, {0x7b}, emit_disp8},
    {"jno", 0, {ARG_VALUE, ARG_NONE}, {0x71}, emit_disp8},
    {"jns", 0, {ARG_VALUE, ARG_NONE}, {0x79}, emit_disp8},
    {"loop", 0, {ARG_VALUE, ARG_NONE}, {0xe2}, emit_disp8},
    {"loopz", "loope", {ARG_VALUE, ARG_NONE}, {0xe1}, emit_disp8},
    {"loopnz", "loopne", {ARG_VALUE, ARG_NONE}, {0xe0}, emit_disp8},
    {"jcxz", 0, {ARG_VALUE, ARG_NONE}, {0xe3}, emit_disp8},
    {"int", 0, {ARG_VALUE, ARG_NONE}, {0xcd}, emit_value8},
    {"into", 0, {ARG_NONE}, {0xce}, emit_simple},
    {"iret", 0, {ARG_NONE}, {0xcf}, emit_simple},
    {"clc", 0, {ARG_NONE}, {0xf8}, emit_simple},
    {"cmc", 0, {ARG_NONE}, {0xf5}, emit_simple},
    {"stc", 0, {ARG_NONE}, {0xf9}, emit_simple},
    {"cld", 0, {ARG_NONE}, {0xfc}, emit_simple},
    {"std", 0, {ARG_NONE}, {0xfd}, emit_simple},
    {"cli", 0, {ARG_NONE}, {0xfa}, emit_simple},
    {"sti", 0, {ARG_NONE}, {0xfb}, emit_simple},
    {"hlt", 0, {ARG_NONE}, {0xf4}, emit_simple},
    {"wait", 0, {ARG_NONE}, {0x9b}, emit_simple},
    {"lock", 0, {ARG_NONE}, {0xf0}, emit_simple},
    {"es", 0, {ARG_NONE}, {0x26}, emit_simple},
    {"cs", 0, {ARG_NONE}, {0x2e}, emit_simple},
    {"ss", 0, {ARG_NONE}, {0x36}, emit_simple},
    {"ds", 0, {ARG_NONE}, {0x3e}, emit_simple},
    {0, 0, 0, 0}
};
