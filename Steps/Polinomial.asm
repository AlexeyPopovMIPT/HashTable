global Polinomial_asm

Polinomial_asm:

    mov si, 3189
    xor rax, rax ; ret val
    xor rcx, rcx ; symb
    mov cl, [rdi]
    jrcxz Return

    Repeat:
        add ax, cx
        inc rdi
        mov cl, [rdi]
        jrcxz Return
        mul si
        jmp Repeat
    Return:
        ret

