#ifndef HANDLER_H_
#define HANDLER_H_

#include <Franklin/datatype.h>

void trap_dividezero();
void trap_pagefault();
void exti_sysclock();
void exti_keyboard();
void exti_rtc();

#endif
