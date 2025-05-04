#ifndef MEMORY_H_
#define MEMORY_H_

/*
    这里要做的就是每当新进程创建时，就得开辟一个二级页表
    当算上该进程的CR3就变成了三级页表
    每有一个新的进程，就得新开辟
    对于CR3而言，一个CR3对应着1024个页目录项；所以为了节省系统因位示图而导致的内存占用过大
    所以，四个进程共享一个CR3
    构建早期内存分配器或者直接对需要的内存进行页表的映射
    在操作系统内核初始化时对内核区的位示图页表进行初始化
    （这是动态的，可以结合loader中的内存检测函数来到指定的位置寻找值，从而动态映射位示图，以适应不同内存的大小）
    映射的页一定是4k对齐的，即加载也是每个页帧的低地址加载，OS设计就是这样（个人的设计），不存在用户进程、宏内核系统进程的半页加载

    个人对于初始化内存的各项数据结构的思路：
    每一个数据结构都是按顺序进行初始化的，因为关于内存的东西都是动态地申请的，并非从一开始就知道
    所以采用类似雨vector的实现方法，通过需要的大小，借助malloc函数来申请页面，从而实现动态对内存单元进行分配
*/
#include <Franklin/datatype.h>
#include <Franklin/head.h>
#include <Franklin/interrupt.h>
#include <Franklin/trap.h>
#include <Franklin/console.h>

/*
    system_tde：对应着系统页目录的存储位置，因为在loader.asm中进行了恒等映射，故一致
    system_tpe：对应着系统第一个页表的存储位置，如果想要扩核，宏内核应当在loader.asm扩；微内核则可以借用操作系统动态扩
    system_loader：这里是将物理内存的第一个1M进行恒等映射，这里有显存的统一编址地址
    访问的线性地址就是所对应的页目录号和页表号，低12位就是页面内的偏移地址（offset）
*/

#define SYSTEM_TDE 0x8000
#define SYSTEM_TPE 0x9000
#define SYSTEM_LOADER 0xa000

/*
    这个在检测完内存会被直接释放内存，所以现在感觉没办法去直接使用0x7c00~0xa0000的内存范围了
    所以直接从0xb000~0x0ffff中申请位示图，如果后期有需要的话，应当将内核映射表直接动态定位到内核区
    但这样造成了比较严重中的碎片化，这是比较严重的问题
*/
#define SYSTEM_DETECTMEM 0xb000

#define CMALLOC_KEY 0x00000000

/*
    这里用来定义有多少个进程共享一个CR3
*/
#define MUTEX_PROCESS 4

/*
    为之后的扩核做准备
*/
#define KERNEL_MEMSIZE 2
#define KERNEL_MEMSIZE_BITMAP (KERNEL_MEMSIZE << 5)

/*
    这里先用宏定义，之后结合内存检测函数来对内存大小进行检测
    #define PHY_MEMSIZE 32
    #define PHY_MEMSIZE_BITMAP (PHY_MEMSIZE << 5)
*/

/*
    用于内存检测的写入数和读出数
*/
#define DETECT_MEMORY_LABLE 0x55aa55aa
#define DETECT_MEMORY_RELABEL 0xaa55aa55

/*
    在内存检测中，每次映射后，都要重复访问的地址，每访问一次代表就检测了4k的内存大小
    4k即一页的大小，要求低12位对齐，少进中断，一次多映射几页
    这里申请的空间，应当在检测完成后，取消申请
*/
#define DETECT_MEM_ADDRESS 0x00200000
#define DETECT_MEM_DIRNUM 0
#define DETECT_MEM_PAGENUM 512

/*
    下面的两个数据结构都是32位的
*/
typedef struct pde
{
    uint8_t P : 1;                 // 存在位 (Present)
    uint8_t R_W : 1;               // 读写权限 (Read/Write)
    uint8_t U_S : 1;               // 用户/超级用户权限 (User/Supervisor)
    uint8_t PWT : 1;               // 写透 (Write-Through)
    uint8_t PCD : 1;               // 缓存禁用 (Cache Disable)
    uint8_t A : 1;                 // 访问位 (Accessed)
    uint8_t reserved1 : 1;         // 保留位 (Reserved)
    uint8_t PS : 1;                // 页大小 (Page Size)
    uint8_t reserved2 : 4;         // 保留位 (Reserved)
    uint32_t page_table_base : 20; // 页表基地址 (Page Table Base Address)
} _packed pde;                     // 页目录项

typedef struct pte
{
    uint8_t P : 1;                 // 存在位
    uint8_t R_W : 1;               // 读写权限
    uint8_t U_S : 1;               // 用户/超级用户
    uint8_t PWT : 1;               // 写透
    uint8_t PCD : 1;               // 缓存禁用
    uint8_t A : 1;                 // 访问位
    uint8_t D : 1;                 // 脏位
    uint8_t PS : 1;                // 页大小
    uint8_t G : 1;                 // 全局页
    uint8_t reserve : 3;           // 保留位
    uint32_t page_frame_base : 20; // 物理页框基地址
} _packed pte;                     // 这个数据结构为四个字节，页表项

/*
    这个系统中，这个数据结构是可以通用的，但一般只运用于物理内存
*/
typedef struct bitmap
{
    uint32_t *bitmapphy_start;
    uint32_t *bitmapline_start;
    size_t bitmap_size;
    size_t label;
} _packed bitmap;

typedef struct memalloc_area
{
    uint32_t *memallocphy_start;
    uint32_t *memallocline_start;
    size_t area_size;
} _packed memalloc_area;

/*
    对系统恒等映射的页目录表和页表的物理地址和逻辑地址
*/
static pde *system_tde = (pde *)SYSTEM_TDE;
static pte *system_tpe = (pte *)SYSTEM_TPE;
static pte *system_detect = (pte *)SYSTEM_DETECTMEM;

/*
    规定系统物理位示图的起始地址都是0x7c00的位置开始
    根据内存检测函数后得到的数据detect_memory_size来判断phy_bitmap.sys_bitmap_end的最终位置
    每次申请都要检查下界，为了更方便的定位到位示图的位置，提供了label这一变量，表示第一个目前未被分配的物理地址
    其实，这里的情况像先有鸡还是先有蛋的样子，目前我只知道那个具体过程，但没法详解描写出来
    直接看代码吧
    线性高地址应当是这些数据结构的访问地址，或者是要求小的时候，完全可以使用前1M的恒等映射
    使用红黑树去管理内存分配区
*/
static bitmap phy_bitmap;
static memalloc_area sys_memalloc_area;

void detect_mem();
void map_page();
void memory_init();

#endif
