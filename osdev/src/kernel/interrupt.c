#include <Franklin/interrupt.h>

void pic_init()
{
    out_byte(host_pic_IMR, 0xff);  // 关闭主PIC上的所有中断响应
    out_byte(slave_pic_IMR, 0xff); // 关闭从PIC上的所有中断响应

    out_byte(host_pic_ICW1, 0x19);
    out_byte(host_pic_ICW2, 0x20); // 0x20~0x27是主PIC的监视范围
    out_byte(host_pic_ICW3, 0x02); // 这里代表只有IRQ2上连接着从PIC
    out_byte(host_pic_ICW4, 0x01);

    out_byte(slave_pic_ICW1, 0x19);
    out_byte(slave_pic_ICW2, 0x28);
    out_byte(slave_pic_ICW3, 0x02); // 表示从PIC连接到主PIC的IRQ2上
    out_byte(slave_pic_ICW4, 0x01);

    out_byte(host_pic_IMR, 0xfd);  // 表示主PIC只开键盘的中断
    out_byte(slave_pic_IMR, 0xff); // 表示从PIC不响应任何外部中断
}

void regist_idt(uint8_t type, uint8_t num, handler_t address)
{
    gate_idt32 *idt_item = &id_table[num];
    idt_item->low_offset = (uint16_t)((uint32_t)address & 0xffff);
    idt_item->segment_selector = (uint16_t)SEGMENT_SELECTOR;
    idt_item->reserved = 0x0;
    idt_item->TYPE = type;
    idt_item->S = 0; // 这里默认为 0 是系统段
    idt_item->DPL = 0x00;
    idt_item->P = 1;
    idt_item->high_offset = (uint16_t)(((uint32_t)address >> 16) & 0xffff);
}

/*
    这个函数用来装载IDT中的内容，这个系统支持256个中断，但系统只提供有限的几个中断服务程序
    想要自己设置中断的话，建议自己再在这里注册中断函数
    注册中断函数的步骤：
    1、需要在handler.asm中添加相对应的响应汇编
    2、在interrupt.h中声明中断函数的名字
    3、在源文件中实现对应得函数
    4、注册中断服务函数
*/
void idt_init()
{
    idtr.limit = (uint16_t)(sizeof(id_table) - 1);
    idtr.base = (uint32_t)&id_table;
    regist_idt(TRAP32_GATE, TRAP_DIVIDEZERO, &trap_dividezero);
    regist_idt(TRAP32_GATE, TRAP_PAGEFAULT, &trap_pagefault);
    regist_idt(EXTI32_GATE, EXTI_SYSCLOCK, &exti_sysclock);
    regist_idt(EXTI32_GATE, EXTI_KEYBOARD, &exti_keyboard);
    regist_idt(EXTI32_GATE, EXTI_RTC, &exti_rtc);
    load_idtr(&idtr);
    store_idtr(&idtr);
}

void interrupt_init()
{
    idt_init();
    pic_init();
}

void send_EOI_host_pic()
{
    out_byte(host_pic_command_port, EOI_message);
}

void send_EOI_slave_pic()
{
    out_byte(slave_pic_command_port, EOI_message);
}