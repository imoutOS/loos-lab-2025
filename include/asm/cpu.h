#ifndef ASM_CPU_H
#define ASM_CPU_H

#include <stdint.h>

/* CSR 读写宏定义 */
// 定义CSR读操作宏
#define DEFINE_CSR_READ(type, name, addr) \
static inline type r_csr_##name() {      \
    type x = 0;                          \
    asm volatile("csrrd %0, " #addr : "=r"(x)); \
    return x;                            \
}

// 定义CSR写操作宏
#define DEFINE_CSR_WRITE(type, name, addr) \
static inline void w_csr_##name(type x) {   \
    asm volatile("csrwr %0, " #addr : : "r"(x)); \
}

/* CSR寄存器定义 */
// ecfg相关
#define CSR_ECFG_VS_SHIFT        16
#define CSR_ECFG_LIE_TI_SHIFT    11
#define HWI_VEC                  0x3fcU
#define TI_VEC                   (0x1 << CSR_ECFG_LIE_TI_SHIFT)
#define CSR_ESTAT_ECODE          (0x3fU << 16)
#define CSR_ESTAT_ESUBCODE       (0x1ffU << 22)

/* 通过宏生成CSR操作函数 */
// CPU配置
static inline uint64_t cpucfg(uint64_t x) {
    uint64_t cfg;
    asm volatile("cpucfg %1, %0" : "=r"(cfg) : "r"(x));
    return cfg;
}

// 中断相关CSR
DEFINE_CSR_READ(uint32_t, crmd, 0x0)
DEFINE_CSR_WRITE(uint32_t, crmd, 0x0)
DEFINE_CSR_WRITE(uint32_t, euen, 0x2)
DEFINE_CSR_READ(uint32_t, ecfg, 0x4)
DEFINE_CSR_WRITE(uint32_t, ecfg, 0x4)
DEFINE_CSR_READ(uint64_t, badv, 0x7)
DEFINE_CSR_READ(uint64_t, badi, 0x8)
DEFINE_CSR_READ(uint32_t, estat, 0x5)
DEFINE_CSR_READ(uint64_t, era, 0x6)
DEFINE_CSR_WRITE(uint64_t, era, 0x6)
DEFINE_CSR_READ(uint64_t, eentry, 0xc)
DEFINE_CSR_WRITE(uint64_t, eentry, 0xc)

// 定时器相关CSR
DEFINE_CSR_READ(uint32_t, tid, 0x40)
DEFINE_CSR_WRITE(uint32_t, tid, 0x40)
DEFINE_CSR_READ(uint32_t, tcfg, 0x41)
DEFINE_CSR_WRITE(uint32_t, tcfg, 0x41)
DEFINE_CSR_READ(uint64_t, tval, 0x42)
DEFINE_CSR_WRITE(uint64_t, tval, 0x42)
DEFINE_CSR_READ(uint32_t, cntc, 0x43)
DEFINE_CSR_WRITE(uint32_t, cntc, 0x43)
DEFINE_CSR_READ(uint32_t, ticlr, 0x44)
DEFINE_CSR_WRITE(uint32_t, ticlr, 0x44)

// TLB相关CSR
DEFINE_CSR_WRITE(uint64_t, tlbrentry, 0x88)
DEFINE_CSR_READ(uint64_t, tlbrbadv, 0x89)
DEFINE_CSR_WRITE(uint64_t, tlbrbadv, 0x89)
DEFINE_CSR_READ(uint64_t, tlbrera, 0x8A)
DEFINE_CSR_WRITE(uint64_t, tlbrelo0, 0x8c)
DEFINE_CSR_WRITE(uint64_t, tlbrelo1, 0x8d)
DEFINE_CSR_WRITE(uint64_t, tlbrehi, 0x8e)
DEFINE_CSR_READ(uint64_t, tlbrehi, 0x8e)

// 内存窗口CSR

#define DMW_PLV0 0x1
#define DMW_PLV1 0x2
#define DMW_PLV2 0x4
#define DMW_PLV3 0x8

DEFINE_CSR_WRITE(uint64_t, dmw0, 0x180)
DEFINE_CSR_READ(uint64_t, dmw0, 0x180)
DEFINE_CSR_WRITE(uint64_t, dmw1, 0x181)
DEFINE_CSR_READ(uint64_t, dmw1, 0x181)
DEFINE_CSR_WRITE(uint64_t, dmw2, 0x182)
DEFINE_CSR_READ(uint64_t, dmw2, 0x182)
DEFINE_CSR_WRITE(uint64_t, dmw3, 0x183)
DEFINE_CSR_READ(uint64_t, dmw3, 0x183)

// 页表相关CSR
DEFINE_CSR_WRITE(uint64_t, pgdl, 0x19)
DEFINE_CSR_READ(uint64_t, pgdl, 0x19)
DEFINE_CSR_READ(uint64_t, pgd, 0x1b)
DEFINE_CSR_WRITE(uint64_t, pgdh, 0x1A)
DEFINE_CSR_READ(uint64_t, pgdh, 0x1A)
DEFINE_CSR_READ(uint64_t, pwcl, 0x1C)
DEFINE_CSR_WRITE(uint64_t, pwcl, 0x1C)
DEFINE_CSR_READ(uint64_t, pwch, 0x1D)
DEFINE_CSR_WRITE(uint64_t, pwch, 0x1D)

DEFINE_CSR_WRITE(uint32_t, stlbps, 0x1E)
DEFINE_CSR_READ(uint32_t, stlbps, 0x1E)
DEFINE_CSR_READ(uint32_t, asid, 0x18)
DEFINE_CSR_WRITE(uint32_t, asid, 0x18)


/* 中断控制函数 */
#define CSR_CRMD_IE_SHIFT 2
#define CSR_CRMD_IE       (0x1 << CSR_CRMD_IE_SHIFT)

static inline int intr_get() {
    return (r_csr_crmd() & CSR_CRMD_IE) != 0;
}

static inline void intr_on() {
    w_csr_crmd(r_csr_crmd() | CSR_CRMD_IE);
}

static inline void intr_off() {
    w_csr_crmd(r_csr_crmd() & ~CSR_CRMD_IE);
}

/* 其他特殊操作 */
static inline void halt(int) {
    intr_off();
    while(1);
}

static inline void tlbfill() {
    asm volatile("tlbfill");
}

static inline void invalidate(void) {
    asm volatile("invtlb 0x0,$r0,$r0");
}

static inline uint64_t r_sp() {
    uint64_t x = 0;
    asm volatile("addi.d %0, $sp, 0" : "=r" (x) );
    return x;
}
static inline void w_sp(uint64_t x) {
    asm volatile("addi.d $sp, %0, 0" : : "r" (x));
}

#endif /* !ASM_CPU_H */
