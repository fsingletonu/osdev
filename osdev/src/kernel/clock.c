#include <Franklin/clock.h>

void exti_sysclock_Handler()
{
    asm volatile("nop\n");
}