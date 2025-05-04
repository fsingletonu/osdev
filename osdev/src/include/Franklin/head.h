#ifndef HEAD_H_
#define HEAD_H_

#include <Franklin/datatype.h>

/*
    用于指定idtr要加载的位置和idtr获取后的状态
*/
typedef struct idtr32
{
    uint16_t limit;
    uint32_t base;
} _packed idtr32;

typedef struct idtr64
{
    uint16_t limit;
    uint64_t base;
} _packed idtr64;

extern void enable();
extern void disable();
extern void en_cache();
extern void un_cache();
extern void load_idtr(idtr32 *address);
extern void store_idtr(idtr32 *store_address);
extern void fiush_tlb(uint32_t *address);

#endif
