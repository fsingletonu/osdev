#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/*
    目前的中断采用PIC进行中断的连接
*/
#include <Franklin/datatype.h>
#include <Franklin/io.h>
#include <Franklin/head.h>
#include <Franklin/handler.h>

/*
PIC 的初始化需要按照固定的顺序写入 4 个初始化命令字（ICW1-ICW4）。以下是初始化的步骤和每个 ICW 的作用：

（1）ICW1：初始化控制字 1
作用：设置 PIC 的基本工作模式。
格式：
Bit 0：是否需要 ICW4（1 = 需要）。
Bit 1：是否级联（0 = 级联模式）。
Bit 2：调用间隔（通常为 0）。
Bit 3：触发模式（0 = 电平触发，1 = 边缘触发）。
Bit 4：标志位（必须为 1）。

（2）ICW2：初始化控制字 2
作用：设置中断向量号（基地址）。
格式：
高 5 位：中断向量号的高 5 位。
低 3 位：由 PIC 自动填充（对应 IRQ 号）。

（3）ICW3：初始化控制字 3
作用：设置级联连接方式。
格式：
主 PIC：每个 bit 表示对应的 IRQ 线是否连接从 PIC。
从 PIC：低 3 位表示连接到主 PIC 的 IRQ 线编号。

（4）ICW4：初始化控制字 4
作用：设置 PIC 的高级功能。
格式：
Bit 0：是否为 8086/8088 模式（1 = 是）。
Bit 1：是否自动结束中断（AEOI，通常为 0）。
Bit 2：是否为缓冲模式（0 = 无缓冲模式）。
Bit 3：是否为特殊全嵌套模式（通常为 0）。

*/

#define host_pic_command_port 0x20
#define host_pic_data_port 0x21
#define slave_pic_command_port 0xa0
#define slave_pic_data_port 0xa1

#define host_pic_IMR host_pic_data_port
#define slave_pic_IMR slave_pic_data_port

#define host_pic_ICW1 host_pic_command_port
#define host_pic_ICW2 host_pic_data_port
#define host_pic_ICW3 host_pic_data_port
#define host_pic_ICW4 host_pic_data_port

#define slave_pic_ICW1 slave_pic_command_port
#define slave_pic_ICW2 slave_pic_data_port
#define slave_pic_ICW3 slave_pic_data_port
#define slave_pic_ICW4 slave_pic_data_port

#define EOI_message 0x20

#define SEGMENT_SELECTOR 8

#define TASK_GATE 0x5
#define EXTI16_GATE 0x6
#define TRAP16_GATE 0x7
#define EXTI32_GATE 0xe
#define TRAP32_GATE 0xf

#define TRAP_DIVIDEZERO 0x00
#define TRAP_PAGEFAULT 0x0e
#define EXTI_SYSCLOCK 0x20
#define EXTI_KEYBOARD 0x21
#define EXTI_RTC 0x28

#define INTERRUPT_NUM 256 // 一共有256个中断

typedef struct gate_idt32
{
    uint16_t low_offset;       // 中断入口函数的低16位
    uint16_t segment_selector; // 段选择子，这里的段选择子必须也是在GDT中声明了的段，否则会触发保护模式的禁忌，直接宕机或者重启。
    uint8_t reserved;          // 保留位，全部都是 0
    uint8_t TYPE : 4;          // 0x5：任务门 0x6：16位中断门 0x7：16位陷阱门 0xe：32位中断门 0xf：32位陷阱门
    uint8_t S : 1;
    uint8_t DPL : 2; // 这2位是为了告知保护模式的CPU，此程序段的权限
    uint8_t P : 1;
    uint16_t high_offset; // 中断入口函数的高16位
} _packed gate_idt32;     // 32位模式下的中断描述符结构

static idtr32 idtr;
static gate_idt32 id_table[INTERRUPT_NUM];

void pic_init();

/*
    这里只有一个系统段，处于平坦模式，故段选择子是与loader中的段选择子是一致
    应当直接固定段选择子
*/
void regist_idt(uint8_t type, uint8_t num, handler_t address);
void idt_init();

void interrupt_init();

void send_EOI_host_pic();
void send_EOI_slave_pic();

/*
    中断服务入口函数（地址）都在这里，用到的文件都需要对interrup.h包含
    这里的函数（地址）直接给regist_idt函数去使用
*/
void trap_dividezero_Handler();
void trap_pagefault_Handler();

void exti_sysclock_Handler();
void exti_keyboard_Handler();
void exti_rtc_Handler();

#endif
