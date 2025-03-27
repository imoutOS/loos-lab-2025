#include <stdint.h>
#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

#define PGSHIFT    14
#define PGSIZE     16384

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define VALEN      48
#define MAXVA      1L << 47
#define PALEN      48
#define GRLEN      64

#define UNCACHED_MEMORY_ADDR 0x8000000000000000
#define MAIN_KERNEL_DMW      0x9000000000000000
#define MAP_PHYSICAL_MEMORY  0xA000000000000000
#define PHYS_TO_UNCACHED(addr) (UNCACHED_MEMORY_ADDR | (addr))
#define PHYS_TO_MAIN(addr)   (MAIN_KERNEL_DMW | (addr))

#define KALLOCBASE 0x9000000080000000
#define KERNBASE   0x9000000090000000

// physical memory

#define MEM1       0x0
#define MMIO1      0x10000000
#define PCIEIO     0x18000000
#define TYPE0      0x1A000000
#define TYPE1      0x1B000000
#define LIOMEM     0x1C000000
#define BOOT       0x1FC00000
#define CONFIG     0x1FE00000
#define SPI        0x1FFF0000
#define MMIO2      0x40000000
#define MEM2       0x80000000
#define PKERNBASE  0x90000000
#define MEM3      0x100000000
#define MEM4      0x200000000
#define MMIO_PCIE 0x400000000

#define TRAPFRAME            MAXVA - PGSIZE
#define V_STACK_SIZE         PGSIZE
#define V_STACK_ADDR         UNCACHED_MEMORY_ADDR - V_STACK_SIZE

// virtual (process) memory
//
#define USER_STACK_TOP       0x7FFF0000
#define USER_STACK_BOTTOM    0x7FF00000

// pmem alloc
#define PMEMALLOC_START      MEM2
#define PMEMALLOC_END        PKERNBASE
#define MEMALLOC_START       PHYS_TO_MAIN(MEM2)
#define MEMALLOC_END         PHYS_TO_MAIN(PKERNBASE)
#define PMEM_AVAIL           (PMEMALLOC_END - PMEMALLOC_START)
// #define PMEM_AVAIL_PAGE      ( PMEM_AVAIL / PGSIZE )
// #define PMEM_AVAIL_PAGE      4096
#define PMEM_AVAIL_PAGE      16384

#define IND_TO_PADDR(X)      (PMEMALLOC_START + X * PGSIZE)

// #define UNCACHED_MEMORY_ADDR 0x8000000000000000
#define UART0                PHYS_TO_UNCACHED(0x1fe20000)
#define SATA_ADDR            PHYS_TO_UNCACHED(0x400e0000)
#define UART0_RHR            (UART0 + 0)
#define UART0_THR            (UART0 + 0)
#define UART0_LSR            (UART0 + 5)
#define LSR_TX_IDLE          (1 << 5)

#define PMC_BASE        PHYS_TO_UNCACHED(0x1fe27000)
#define REBOOT_ADDR     (PMC_BASE + 0x30)
#define REBOOT_MASK     0x1
#define POWEROFF_ADDR   (PMC_BASE + 0x14)
#define POWEROFF_MASK   0x3c00

// #define KERNEND    0x10000000 + 0x9000000000000000

// loaded kernelbase is 0x9000000008000000, but the preset DWM map this to
// 0x8000000

typedef uint64_t *pagetable_t;
typedef uint64_t pte_t;


// CSR.PWCL和CSR.PWCH用来配置LDDIR和LDPTE指令所遍历页表的规格参数信息，其中
// CSR.PWCL中定义了每个页表项的宽度（PTEwidth域）以及末级页表索引的起始位置和
// 位宽（PTbase和PTwidth域）、页目录表1索引的起始位置和位宽（Dir1_base和
// Dir1_width域）、页目录表2索引的起始位置和位宽（Dir2_base和Dir2_width域）,
// CSR.PWCH中定义了页目录表3索引的起始位置和位宽（Dir3_base和Dir3_width域）、
// 页目录表4索引的起始位置和位宽（Dir4_base和Dir4_width域）。在
// Linux/LoongArch64中，当进行三级页表的遍历时，通常用Dir1_base和Dir1_width域
// 来配置页目录表PMD索引的起始位置和位宽，用Dir3_base和Dir3_width域来配置页目
// 录表PGD索引的起始位置和位宽，Dir2_base和Dir2_width域、Dir4_base和Dir4_width
// 域空闲不用。

#define DIR1BASE   25
#define DIR1WIDTH  11
#define DIR3BASE   36
#define DIR3WIDTH  11
#define PTBASE     14
#define PTWIDTH    11

#define DIR2BASE   0
#define DIR2WIDTH  0
#define DIR4BASE   0
#define DIR4WIDTH  0
#define PTEWIDTH   0

#define PXMASK     0x7ff // 11 bits
#define PXSHIFT(level)  (PGSHIFT+(11*(level)))
#define PX(level, va) ((((uint64_t) (va)) >> PXSHIFT(level)) & PXMASK)
#define DMWIN_MASK 0xA000000000000000

#endif /* !MEMLAYOUT_H */
