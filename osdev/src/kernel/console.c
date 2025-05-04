#include <Franklin/console.h>

void test_io()
{
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);
    uint8_t val = in_byte(CRT_ADDR_REG);
}

/*
    这个函数是将显存第一行直接指向显存开始的第一行，用于滚屏的操作
    screen的值已由调用函数直接修改，这里直接解析即可
*/
void set_screen()
{
    uint16_t screen_pos = (uint16_t)(screen - MEM_BASE);
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);
    out_byte(CRT_DATA_REG, screen_pos >> 8);
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);
    out_byte(CRT_DATA_REG, screen_pos);
}

/*
    这个里面存的是在彩色文本模式下相对于0xb8000的偏移量，每加80(D)就是向上移一行，每减80(D)就是向下移一行
*/
void get_screen()
{
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);
    screen = in_byte(CRT_DATA_REG) << 8;
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);
    screen |= in_byte(CRT_DATA_REG);
    screen <<= 1;
    screen += MEM_BASE;
}

/*
    用于光标位置的设置，x与y的值已由调用函数直接修改，这里直接解析即可
    这里的值为0~1999
*/
void set_cursor()
{
    uint16_t pos = (uint16_t)(y * 80 + x);
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);
    out_byte(CRT_DATA_REG, pos >> 8);
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);
    out_byte(CRT_DATA_REG, pos);
}

/*
    获取当前的光标位置，光标寄存器中存的是当前页面中的偏移量
*/
void get_cursor()
{
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);
    cursor_pos = in_byte(CRT_DATA_REG) << 8;
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);
    cursor_pos |= in_byte(CRT_DATA_REG);
    x = cursor_pos % WIDTH;
    y = cursor_pos / WIDTH;
    cursor_pos <<= 1;
    cursor_pos += MEM_BASE;
}

/*
    主要用于测试
*/
void write_halfword()
{
    uint16_t *vga_ptr = (uint16_t *)cursor_pos;
    *vga_ptr = (uint16_t)(BASE_FB << 8) | 0x00;
}

/*
    进行屏幕上滚的效果
    个人有一个逻辑，看下一个输出的字符的y*80+x是否在0~1999这个范围内
    如果在这个范围内，直接退出该上滚函数；如果不在这个范围内，则需要模2000以回到该范围内
    这个可以根据指针的位置进行设置，简化操作，反正人眼又看不见
*/
static void scroll_up()
{
    if (y * 80 + x > 1920)
    {
        screen += WIDTH;
        y = HEIGHT - 1;
        set_screen();
    }
}

/*
    主要用于输出
    第一个参数为字符串，第二个字符为字符串长度，传入参数为sizeof(string)-1
    当这个函数响应，应当先检查是否超过当前的80*25的大小范围和重新获取当前指令，如果超过，须向上滚；如果未超过，则正常进行
    一切对于光标更改的操作都要在显示之后，遇到\n则直接加到下一个80倍数，这样可以优化因读取端口而产生的开销
    映射方式占空间
    *这里多色字体有问题，最后在来解决*
*/
void write_string(char *string, size_t length)
{
    get_cursor();
    uint32_t offset = 0;
    uint16_t *vga_ptr = (uint16_t *)cursor_pos;
    for (size_t i = 0; i < length; i++, vga_ptr++, string++)
    {
        switch (*string)
        {
        case NUL:
            *vga_ptr = (uint16_t)(BASE_FB << 8) | 0x20;
            break;

        case ESC:
            break;

        case BEL:
            break;

        case BS:
            break;

        case HT:
            for (size_t i = 0; i <= 4; i++)
            {
                *vga_ptr = (uint16_t)(BASE_FB << 8) | 0x20;
                vga_ptr++;
            }
            x += 4;
            break;

        case LF:
            offset = x;
            x += WIDTH;
            x -= (x % WIDTH);
            offset = x - offset;
            y += x / WIDTH;
            x %= WIDTH;
            vga_ptr += (offset - 1);
            set_cursor();
            break;

        case VT:
            break;

        case FF:
            break;

        case CR:
            break;

        case DEL:
            break;

        default:
            *vga_ptr = (uint16_t)(BASE_FB << 8) | (uint8_t)*string;
            x++;
            break;
        }
    }
    scroll_up();
}

/*
    将VGA显卡的内存统一映射区全部清零，因内存地址超过16位，故直接使用32位，不想去弄间接寻址
*/
void console_clear()
{
    x = 0;
    y = 0;
    screen = MEM_BASE;
    set_cursor();
    set_screen();
    for (uint32_t *vga_ptr = (uint32_t *)MEM_BASE; vga_ptr != (uint32_t *)MEM_END; vga_ptr++)
    {
        *vga_ptr = (erase << 16) | erase;
    }
}

void console_init()
{
    console_clear();
}