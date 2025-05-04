#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <Franklin/datatype.h>
#include <Franklin/io.h>

/*
    以下两个寄存器的作用机制是：
    CRT_ADDR是指定显卡中的显存寄存器的地址的。
    CRT_DATA是指定显卡中的已经被CRT_ADDR指定的显存寄存器的内容的。
    在这里，每个字符由两个字节控制，前一个字节用来写ASCII码值，后一个字节用来控制前一个ASCII值的样式。
    第一个字节（ASCII码/字符编码）
    存储字符的ASCII值（或扩展字符集编码），决定屏幕上显示的符号（如字母、数字等）。
    第二个字节（属性字节）
    定义字符的显示属性，包括：
    前景色（低4位）：字符颜色（16色可选）。
    背景色（高3位 + 闪烁位）：背景颜色（通常8色，若启用闪烁则背景色减少至7色）。
    闪烁控制（最高位）：控制字符是否闪烁（传统VGA特性）。
    CURSOR（0~1999）
    SCREEN（相对于0xb8000偏移量），所以在写代码的过程中应当注意加减0xb8000

    彩色文本模式的统一内存地址为0xb8000~0xb8f9f
    尽可能地少读写内存
*/
#define CRT_ADDR_REG 0x3d4 // CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3d5 // CRT(6845)数据寄存器

#define CRT_START_ADDR_H 0xc // 显示内存起始位置 - 高位
#define CRT_START_ADDR_L 0xd // 显示内存起始位置 - 低位
#define CRT_CURSOR_H 0xe     // 光标位置 - 高位
#define CRT_CURSOR_L 0xf     // 光标位置 - 低位

#define MEM_BASE 0x000b8000           // 显卡内存起始位置
#define MEM_SIZE 0x4000               // 显卡内存大小
#define MEM_END (MEM_BASE + MEM_SIZE) // 显卡内存结束位置
#define WIDTH 80                      // 屏幕文本列数
#define HEIGHT 25                     // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2)          // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // 屏幕字节数

/*
    前景色（低四位）
*/
#define FRONT_BLACK 0x00
#define FRONT_BLUE 0x01
#define FRONT_GREEN 0x02
#define FRONT_CYAN 0x03
#define FRONT_RED 0x04
#define FRONT_MAGENTA 0x05 // 品红
#define FRONT_BROWN 0x06
#define FRONT_LIGHTGRAY 0x07
#define FRONT_DARKGRAY 0x08
#define FRONT_BRIGHTBLUE 0x09
#define FRONT_BRIGHTGREEN 0x0a
#define FRONT_BRIGHTCYAN 0x0b
#define FRONT_BRIGHTRED 0x0c
#define FRONT_BRIGHTMAGENTA 0x0d
#define FRONT_YELLOW 0x0e
#define FRONT_WHITE 0x0f

/*
    背景色（中三位），最高一位直接为 0 ，不闪烁
*/
#define BACKGROUND_BLACK 0x00
#define BACKGROUND_BLUE 0x01
#define BACKGROUND_GREEN 0x02
#define BACKGROUND_CYAN 0x03
#define BACKGROUND_RED 0x04
#define BACKGROUND_MAGENTA 0x05
#define BACKGROUND_BROWN 0x06
#define BACKGROUND_LIGHTGRAY 0x07

/*
    默认前景与背景颜色组合
*/
#define BASE_FB (BACKGROUND_BLACK << 4) | FRONT_LIGHTGRAY

/*
    下面的是特殊字符的ASCII码，控制字符需要有自己的处理逻辑才可正确显示
*/
#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1b // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0a  // \n
#define VT 0x0b  // \v
#define FF 0x0c  // \f
#define CR 0x0d  // \r
#define DEL 0x7f

uint32_t screen;     // 当前显示器开始的位置
uint32_t cursor_pos; // 当前光标
uint32_t x, y;       // 文本模式下光标的位置
static uint16_t erase = 0x0720;

void test_io();
void set_screen();
void set_cursor();
void get_screen();
void get_cursor();
void write_halfword();
void write_string(char *string, size_t length);
void console_init();

#endif
