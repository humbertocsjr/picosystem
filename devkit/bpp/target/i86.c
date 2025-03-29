#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

void error(char *fmt, ...);
void out(char *fmt, ...);
int new_label();
int do_eq = 0;
int do_ne = 0;
int do_ult = 0;
int do_slt = 0;
int do_ule = 0;
int do_sle = 0;
int do_ugt = 0;
int do_sgt = 0;
int do_uge = 0;
int do_sge = 0;

char *cg_get_target_prefix()
{
    return "i86";
}

void cg_init()
{
    
}

void cg_header()
{
    out("section .text");
}

void embed_eq()
{
    int lbl = new_label();
    out("eq:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jz L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_ne()
{
    int lbl = new_label();
    out("ne:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jnz L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_ult()
{
    int lbl = new_label();
    out("ult:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jb L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_slt()
{
    int lbl = new_label();
    out("slt:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jl L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_ule()
{
    int lbl = new_label();
    out("ule:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jbe L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_sle()
{
    int lbl = new_label();
    out("sle:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jle L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_ugt()
{
    int lbl = new_label();
    out("ugt:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("ja L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_sgt()
{
    int lbl = new_label();
    out("sgt:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jg L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_uge()
{
    int lbl = new_label();
    out("uge:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jae L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void embed_sge()
{
    int lbl = new_label();
    out("sge:");
    out("cmp ax, cx");
    out("mov ax, 1");
    out("jge L%d", lbl);
    out("dec ax");
    out("L%d:", lbl);
    out("ret");
}

void cg_footer()
{
    out("; STANDARD LIBRARY");
    if(do_eq) embed_eq();
    if(do_ne) embed_ne();
    if(do_ult) embed_ult();
    if(do_ule) embed_ule();
    if(do_ugt) embed_ugt();
    if(do_uge) embed_uge();
    if(do_slt) embed_slt();
    if(do_sle) embed_sle();
    if(do_sgt) embed_sgt();
    if(do_sge) embed_sge();
}

void cg_global_var(char *name, int value)
{
    out("section .data");
    out("_%s:", name);
    out("dw %d", value);
    out("section .text");
}

void cg_global_bytearr(char *name, int size)
{
    out("section .data");
    out("_%s:", name);
    out("dw _%s + 2", name);
    out("times %d db 0", size);
    out("section .text");
}

void cg_global_wordarr(char *name, int size)
{
    out("section .data");
    out("_%s:", name);
    out("dw _%s + 2", name);
    out("times %d dw 0", size);
    out("section .text");
}

void cg_local_bytearr(char *name, int size)
{
    out("sub sp, %d", size);
    out("mov ax, sp");
    out("push ax");
}

void cg_local_wordarr(char *name, int size)
{
    out("sub sp, %d", size * 2);
    out("mov ax, sp");
    out("push ax");
}

void cg_set_acc(int value)
{
    out("mov ax, %d", value);
}

void cg_set_aux(int value)
{
    out("mov cx, %d", value);
}

void cg_push_acc()
{
    out("push ax");
}

void cg_push_aux()
{
    out("push cx");
}

void cg_pop_acc()
{
    out("pop ax");
}

void cg_pop_aux()
{
    out("pop cx");
}

void cg_function(char *name)
{
    out("global _%s", name);
    out("_%s:", name);
    out("push bp");
    out("mov bp, sp");
}

void cg_end_function()
{
    out("mov sp, bp");
    out("pop bp");
    out("ret");
}

void cg_reserve_stack(int size)
{
    if(size == 0) return;
    out("sub sp, %d", size);
}

void cg_restore_stack(int size)
{
    if(size == 0) return;
    out("add sp, %d", size);
}

int cg_get_auto_size()
{
    return 2;
}

int cg_get_args_offset()
{
    return 4;
}

int cg_get_vars_offset()
{
    return 0;
}

char *cg_get_comment_start()
{
    return "        ; ";
}

void cg_store_local(char *name, int offset)
{
    out("mov [bp+%d], ax ; WRITE %s", offset, name);
}

void cg_store_global(char *name)
{
    out("mov [_%s], ax ; WRITE %s", name, name);
}

void cg_addr_local(char *name, int offset)
{
    out("lea ax, [bp+%d] ; ADDR %s", offset, name);
}

void cg_addr_global(char *name)
{
    out("lea ax, [_%s] ; ADDR %s", name, name);
}

void cg_load_local(char *name, int offset)
{
    out("mov ax, [bp+%d] ; READ %s", offset, name);
}

void cg_load_global(char *name)
{
    out("mov ax, [_%s] ; READ %s", name, name);
}

void cg_load_local_to_aux(char *name, int offset)
{
    out("mov cx, [bp+%d] ; READ %s", offset, name);
}

void cg_load_global_to_aux(char *name)
{
    out("mov cx, [_%s] ; READ %s", name, name);
}

void cg_store_byte()
{
    out("mov si, ax");
    out("mov [si], cl");
}

void cg_store_word()
{
    out("mov si, ax");
    out("mov [si], cx");
}

void cg_load_byte()
{
    out("mov si, ax");
    out("mov al, [si]");
    out("xor ah, ah");
}

void cg_load_word()
{
    out("mov si, ax");
    out("mov ax, [si]");
}

void cg_add()
{
    out("add ax, cx");
}

void cg_call_acc()
{
    out("call ax");
}

void cg_call(char *name)
{
    out("call _%s", name);
}

void cg_sub()
{
    out("sub ax, cx");
}

void cg_udiv()
{
    out("xor dx, dx");
    out("div cx");
}

void cg_sdiv()
{
    out("xor dx, dx");
    out("idiv cx");
}

void cg_mod()
{
    out("xor dx, dx");
    out("div cx");
    out("mov ax, dx");
}

void cg_umul()
{
    out("mul cx");
}

void cg_smul()
{
    out("imul cx");
}

void cg_eq()
{
    out("call eq");
    do_eq = 1;
}

void cg_ne()
{
    out("call ne");
    do_ne = 1;
}

void cg_ult()
{
    out("call ult");
    do_ult = 1;
}

void cg_slt()
{
    out("call slt");
    do_slt = 1;
}

void cg_ule()
{
    out("call ule");
    do_ule = 1;
}

void cg_sle()
{
    out("call sle");
    do_sle = 1;
}

void cg_ugt()
{
    out("call ugt");
    do_ugt = 1;
}

void cg_sgt()
{
    out("call sgt");
    do_sgt = 1;
}

void cg_uge()
{
    out("call uge");
    do_uge = 1;
}

void cg_sge()
{
    out("call sge");
    do_sge = 1;
}

void cg_jump(int lbl)
{
    out("jmp L%d", lbl);
}

void cg_jump_if_true(int lbl)
{
    int lbl_skip = new_label();
    out("cmp ax, 0");
    out("jz L%d", lbl_skip);
    out("jmp L%d", lbl);
    out("L%d:", lbl_skip);
}

void cg_jump_if_false(int lbl)
{
    int lbl_skip = new_label();
    out("cmp ax, 0");
    out("jnz L%d", lbl_skip);
    out("jmp L%d", lbl);
    out("L%d:", lbl_skip);
}

void cg_label(int lbl)
{
    out("L%d:", lbl);
}

void cg_set_acc_string(char *str)
{
    int lbl = new_label();
    out("section .data");
    out("L%d:", lbl);
    while(*str)
    {
        out("db %d ; '%c'", *str, (*str >= ' ' && *str <= 127) ? *str : ' ');
        str++;
    }
    out("db 0");
    out("section .text");
    out("mov ax, L%d", lbl);
}

void cg_inc()
{
    out("inc ax");
}

void cg_dec()
{
    out("dec ax");
}

void cg_xor()
{
    out("xor ax, cx");
}

void cg_or()
{
    out("or ax, cx");
}

void cg_and()
{
    out("and ax, cx");
}

void cg_shl()
{
    out("shl ax, cl");
}

void cg_shr()
{
    out("shr ax, cl");
}

void cg_load_far_byte()
{
    out("push ds");
    out("mov ds, ax");
    out("mov si, cx");
    out("mov al, [si]");
    out("xor ah, ah");
    out("pop ds");
}

void cg_load_far_word()
{
    out("push ds");
    out("mov ds, ax");
    out("mov si, cx");
    out("mov ax, [si]");
    out("pop ds");
}

void cg_store_far_byte()
{
    out("pop bx");
    out("push ds");
    out("mov ds, ax");
    out("mov si, cx");
    out("mov [si], bl");
    out("pop ds");
}

void cg_store_far_word()
{
    out("pop bx");
    out("push ds");
    out("mov ds, ax");
    out("mov si, cx");
    out("mov [si], bx");
    out("pop ds");
}


