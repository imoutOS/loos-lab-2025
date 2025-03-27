#ifndef ASM_MMU_H
#define ASM_MMU_H

#define PTE_V      1 << 0
#define PTE_D      1 << 1
#define PTE_PLVL   1 << 2
#define PTE_PLVH   1 << 3
#define PTE_PLV    3 << 2
#define PTE_MATL   1 << 4
#define PTE_MATH   1 << 5
#define PTE_G      1 << 6
#define PTE_P      1 << 7
#define PTE_W      1 << 8
#define PTE_NR     1 << 61
#define PTE_NX     1 << 62
#define PTE_RPLV   1 << 63
#define PTE_DEFAULT PTE_V | PTE_MATL | PTE_P | PTE_W | PTE_D
#define PTE_FLAGS(pte) ((pte) & 0xE0000000000001FFUL)

#define PAMASK     0xFFFFFFFFFUL << PGSHIFT
#define PTE2PA(pte) (pte & PAMASK)
#define PA2PTE(pa) (((uint64_t)pa) & PAMASK)

// #define PA2VA(pa) (((uint64_t)pa) | DMWIN_MASK)
// #define VA2PA(va) ((((uint64_t)va) & DMWIN_MASK) ? (((uint64_t)va) & PAMASK) : ((uint64_t)va))

#define PA2VA(pa) (((uint64_t)pa) | MAIN_KERNEL_DMW)
#define VA2PA(va) ((((uint64_t)va) & MAIN_KERNEL_DMW) ? (((uint64_t)va) & PAMASK) : ((uint64_t)va))

// void mm_reset(uint64_t addr) {
    // asm volatile("cacop 0x0, %0, 0" : : "r" (addr));
    // asm volatile("cacop 0x1, %0, 0" : : "r" (addr));
    // asm volatile("cacop 0x2, %0, 0" : : "r" (addr));
    // asm volatile("cacop 0x3, %0, 0" : : "r" (addr));
    // asm volatile("ibar 0");
    // asm volatile("dbar 0");
    // tlbinv_all();
// }


#endif /* !ASM_MMU_H */
