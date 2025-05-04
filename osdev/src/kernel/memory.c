#include <Franklin/memory.h>

/*
    这个文件中包含着对内存管理的全部操作
*/

char detect[] = "detecting memory!!!";
static size_t detect_memory_size = 0;

/*
    一个页目录指向一个页表
    检测内存大小的函数，每次检测4k的第一个位置，每次的偏移量加4k
    c的编译器应该会重载
    目前应当检测的值为8192才对
    检测过程中，遇到的新问题：
    1、0xA0000-0xFFFFF是硬件预留区，只读不写，如果直接读的话，读出的是0；直接写，是写不进去的，
       又因为显存等都在这里，直接不检低1M物理内存，正好，不稀罕loader的区域（内核在物理地址的1M开始）
    2、在写入页表、页目录项时，不应当立即访问内存，要刷新快表（针对间隔时间很短的访问同一个线性地址）
       如果，访问其它线性地址，快表中可能没有，便会直接访问内存中的表，不会造成脏读
       // asm volatile("invlpg (%0)" : : "r"(detect_address));这个是AT&T语法的invlpg汇编，这个效率更高，但我还是喜欢intel
    3、TMD，为什么这硬件通过页表访问无效物理地址不会出现缺页中断，只看P位啊，我以为x86有多NB呢
*/
void detect_mem()
{
    write_string(detect, sizeof(detect) - 1);
    uint32_t store;

    // 这里就是将内存在系统的页表中申请，这里是需要被映射的物理内存地址
    uint32_t phy_address = 0x00100000;
    uint32_t *detect_address = (uint32_t *)DETECT_MEM_ADDRESS;

    // 访问的线性地址为0x00020000，内存前1M是恒等映射，可以直接访问
    // 这里一致逻辑错的原因是快表没有更新，导致出问题
    uint32_t *sys_detect_tpe = (uint32_t *)(SYSTEM_LOADER + 4 * DETECT_MEM_PAGENUM);
    while (true)
    {
        *sys_detect_tpe = (phy_address | 0x03);
        fiush_tlb(detect_address);
        store = *detect_address;
        *detect_address = (uint32_t)DETECT_MEMORY_LABLE;
        if (~(*detect_address) != DETECT_MEMORY_RELABEL)
        {
            // 将申请到的内存释放
            *detect_address = (uint32_t)CMALLOC_KEY;
            break;
        }
        *detect_address = store;
        phy_address += 0x1000;
        detect_memory_size++;
    }
}

void memalloc_area_init()
{
    // sys_memalloc_area.memallocphy_start=;
    // sys_memalloc_area.memallocline_start=;
    // sys_memalloc_area.area_size=;
}

/*
    这个函数是为了对整个内存进行位示图的映射，一位代表了个页，这里为了省事，直接在原本loader的内存位置上直接向后分配
    
*/
void bitmap_phymem_init()
{
    
    asm volatile("nop\n");
}

/*
    这个函数统一对物理内存进行页式映射
*/
void map_page()
{
}

void memory_init()
{
    detect_mem();
    bitmap_phymem_init();
}
