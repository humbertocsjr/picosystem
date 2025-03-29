section .text
LOADER equ 0x7c00
BUFFER equ 0x7e00
ITEM equ 0x600
PFS_BLOCK equ 500
PFS_NEXT equ 502
PFS_DATA equ 506
PFS_RESOURCES equ 508
PFS_SIGNATURE equ 0x8919

global _start
_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti
    mov [var_disk], dl
    mov [var_heads], ax
    mov [var_sects], ax
    int 0x12
    cmp ax, 189
    jae .mem_ok
        call error
        db "MEM",0
    .mem_ok:
    mov cx, 64
    sub ax, 16
    mul cx
    mov [var_dest], ax
    mov [var_exec], ax
    mov si, floppy_table
    mov dl, [var_disk]
    cmp dl, 0x80
    jb .floppy
        jmp init_hd
    .floppy:
        xor ax, ax
        int 0x13
        mov bx, BUFFER
        mov ax, 0x201
        mov cl, [si]
        xor ch, ch
        mov dh, [si+1]
        dec dh
        int 0x13
        jnc .ok
        inc si
        inc si
        cmp byte [si], 0
        jne .floppy
    call error
    db "FLP",0
    .ok:
    mov ax, [si]
    jmp init

init_hd:
    mov ah, 8
    mov dl, [var_disk]
    int 0x13
    jc .fail
        and cl, 0x3f
        mov al, cl
        mov ah, dh
        jmp init
    .fail:
    call error
    db "HD",0

init:
    mov [var_sects], al
    mov [var_heads], ah
    mov ax, [LOADER + PFS_NEXT]
    mov bx, BUFFER
    call disk_read
    mov ax, [BUFFER + PFS_DATA]
    .find:
        mov bx, BUFFER
        call disk_read
        mov si, BUFFER
        mov di, name_file
        mov cx, 64
        .comp:
            cmpsb
            jne .next
            dec si
            lodsb
            cmp al, 0
            je .found
            loop .comp
        .next:
        mov ax, [BUFFER + PFS_NEXT]
        cmp ax, 0
        jne .find
    .not_found:
        call error
        db "NOT FOUND",0
    .found:
    mov ax, [BUFFER + PFS_DATA]
    cmp ax, 0
    jne .not_empty
        call error
        db "DATA",0
    .not_empty:
    mov bx, BUFFER
    .load:
        call disk_read
        mov cx, 500
        mov di, [var_dest]
        mov es, di
        mov di, [var_offset]
        mov si, BUFFER
        rep
        movsb
        mov ax, ds
        mov es, ax
        mov ax, [var_offset]
        add ax, 500
        mov cx, ax
        shr cx, 1
        shr cx, 1
        shr cx, 1
        shr cx, 1
        add [var_dest], cx
        and ax, 0xf
        mov [var_offset], ax
        mov ax, [BUFFER + PFS_NEXT]
        cmp ax, 0
        je .done
        mov bx, BUFFER
        jmp .load
    .done:
    mov si, [var_exec]
    mov es, si
    es
    mov ax, [10]
    inc si
    push si
    push ax
    mov ax, [var_disk]
    mov bx, [var_heads]
    mov cx, [var_sects]
    retf

; ax = lba
; bx = dest
disk_read:
    push si
    mov cx, 3
    .try:
        push ax
        push bx
        push cx
        xor dx, dx
        div word [var_sects]
        mov cl, dl
        inc cl
        xor dx, dx
        div word [var_heads]
        mov dh, dl
        mov dl, [var_disk]
        mov ch, al
        ror ah, 1
        ror ah, 1
        and ah, 0xc0
        or cl, ah
        mov ax, 0x201
        int 0x13
        jnc .end
        xor ax, ax
        mov dl, [var_disk]
        int 0x13
        pop cx
        pop bx
        pop ax
        loop .try
        call error
        db "DISK",0
    .end:
    pop cx
    pop bx
    pop ax
    pop si
    ret

error:
    mov bp, sp
    mov si, [bp]
    .loop:
        lodsb
        cmp al, 0
        je halt
        mov ah, 0xe
        int 0x10
        jmp .loop
    mov ax, 0xe00 + ' '
    int 0x10
    mov ax, 0xe00 + 'E'
    int 0x10
    mov ax, 0xe00 + 'R'
    int 0x10
    mov ax, 0xe00 + 'R'
    int 0x10
halt:
    hlt
    jmp halt
section .data
floppy_table:
    db 36, 2
    db 18, 2
    db 15, 2
    db 9, 2
    db 9, 1
    db 0, 0

var_heads: dw 0
var_sects: dw 0
var_disk: db 0
var_exec: dw 0x1000
var_dest: dw 0x1000
var_offset: dw 0
name_file:
    db "Loader",0