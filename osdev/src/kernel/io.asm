[bits 32]
section .text

global in_byte
global in_word
global out_byte
global out_word

; 在32位模式下，leave指令相当于mov esp, ebp\n pop ebp\n
; intel的CPU硬件采用的是递减栈，即栈顶在低地址上
; GCC 默认使用 cdecl 调用约定（在 32 位模式下），需要开发者自己去将栈指针重新指向调用前的栈指针
; 用如下得格式去调用参数，那ebp+8是第一个参数，ebp+12是第二个参数，后续的多参数以此类推

in_byte:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    in al,dx

    jmp $+2
    jmp $+2
    jmp $+2

    pop edx

    leave
    ret

in_word:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    in ax,dx

    jmp $+2
    jmp $+2
    jmp $+2

    pop edx

    leave
    ret

out_byte:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,al

    jmp $+2
    jmp $+2
    jmp $+2

    pop edx

    leave
    ret

out_word:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    mov eax,[ebp+12]
    out dx,ax

    jmp $+2
    jmp $+2
    jmp $+2

    pop edx

    leave
    ret