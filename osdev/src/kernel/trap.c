#include <Franklin/trap.h>

void trap_dividezero_Handler()
{
    asm volatile("nop\n");
}

/*
    asm volatile(
        "movl 0x20000000, %eax\n\t"  // 把无效地址放到 eax
        "movl (%eax), %eax\n\t"      // 尝试读取这个无效地址，触发页异常
    );
    以上的AT&T内嵌汇编指令能够触发这里的Trap
*/
void trap_pagefault_Handler()
{
    asm volatile("nop");
}