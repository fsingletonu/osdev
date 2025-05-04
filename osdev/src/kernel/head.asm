[bits 32]
section .text

; 传值的原则是累加寄存器中放值
global enable
global disable
global en_cache
global un_cache
global load_idtr
global store_idtr
global fiush_tlb

enable:
    sti
    ret

disable:
    cli
    ret

; 开启缓存
en_cache:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax

    mov eax,cr0
    and eax,0xbfffffff
    mov cr0,eax

    pop edx

    leave
    ret

;关闭缓存
un_cache:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax

    mov eax,cr0
    or eax,0x40000000
    mov cr0,eax

    pop edx

    leave
    ret

load_idtr:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    lidt [edx]

    pop edx

    leave
    ret

store_idtr:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    sidt [edx]

    pop edx

    leave
    ret

fiush_tlb:
    push ebp
    mov ebp,esp

    push edx

    xor eax,eax
    mov edx,[ebp+8]
    invlpg [edx]
    
    pop edx
    leave
    ret