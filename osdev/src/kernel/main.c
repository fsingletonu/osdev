#include <Franklin/Franklin.h>

char message[] = "Hello World!!!\n";

void kernel_init()
{
    // 初始化系统原子区禁止中断（这里往后就不需要在其它初始化函数中去调用开关中断）
    disable();

    console_init();
    interrupt_init();
    write_string(message, sizeof(message) - 1);
    memory_init();

    // 初始化系统原子区开启中断
    enable();
    while (true)
    {
        
    }
}