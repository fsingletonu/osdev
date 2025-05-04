#ifndef TRAP_H_
#define TRAP_H_

#include <Franklin/datatype.h>
#include <Franklin/interrupt.h>

static bool label_detecting = true;

void trap_dividezero_Handler();
void trap_pagefault_Handler();

#endif
