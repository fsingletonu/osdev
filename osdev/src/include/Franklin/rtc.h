#ifndef RTC_H_
#define RTC_H_

#include <Franklin/datatype.h>
#include <Franklin/interrupt.h>

/*
    这里采用的是CMOS芯片
    它有两种中断，一、周期性中断（更新时间，显示时间） 二、闹钟中断（嵌入式设备的唤醒等）

*/

#define select_register 0x70
#define data_register 0x71

#endif
