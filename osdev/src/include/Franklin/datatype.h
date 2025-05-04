#ifndef DATATYPE_H_
#define DATATYPE_H_

#define EOF -1

#define NULL 0

#define EOS '\0'

#define true 1
#define false 0

#define page_aligned 4096

// 这里的是100k的位置
#define memory_bitmap_address 0x29000
#define memory_pool_address

#define _packed __attribute__((packed))
#define _page_aligned __attribute__((aligned(page_aligned)))

// 这个特性好像不起作用
#define _memory_bitmap __attribute__((at(memory_bitmap_address)))
#define _memory_pool __attribute__((at(memory_pool_address)))

typedef unsigned char bool;

typedef unsigned int size_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef void* handler_t;

#endif