[bits 32]
section .text

global trap_dividezero ; 0x00 divide zero
global trap_pagefault ; 0x0e page fault

global exti_sysclock ; 0x20 system clock which is used to switch task
global exti_keyboard ; 0x21 keyboard input
global exti_rtc ; 0x28 real time

extern trap_dividezero_Handler
extern trap_pagefault_Handler
extern exti_sysclock_Handler
extern exti_keyboard_Handler
extern exti_rtc_Handler

trap_dividezero:
    pushad
    push fs
    push ds
    push es
    push ss
    push gs

    call trap_dividezero_Handler

    pop gs
    pop ss
    pop es
    pop ds
    pop fs
    popad
    iret

trap_pagefault:
    pushad
    push fs
    push ds
    push es
    push ss
    push gs

    call trap_pagefault_Handler

    pop gs
    pop ss
    pop es
    pop ds
    pop fs
    popad
    iret

exti_sysclock:
    pushad
    push fs
    push ds
    push es
    push ss
    push gs

    call exti_sysclock_Handler

    pop gs
    pop ss
    pop es
    pop ds
    pop fs
    popad
    iret

exti_keyboard:
    pushad
    push fs
    push ds
    push es
    push ss
    push gs

    call exti_keyboard_Handler

    pop gs
    pop ss
    pop es
    pop ds
    pop fs
    popad
    iret

exti_rtc:
    pushad
    push fs
    push ds
    push es
    push ss
    push gs

    call exti_rtc_Handler

    pop gs
    pop ss
    pop es
    pop ds
    pop fs
    popad
    iret