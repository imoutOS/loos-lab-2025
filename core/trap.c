#include "klib.h"
#include "trap.h"
#include "syscall.h"
#include "memlayout.h"
#include "asm/cpu.h"
#include "asm/exception.h"
#include "proc.h"
#include "driver.h"
#include "time.h"

void handle_tlbr();

static inline uint64_t get_dmw_data(uint32_t base, uint8_t mat, uint8_t plv) {
    uint64_t res = ((uint64_t)base << VALEN) | ((uint64_t)mat << 4) | (uint64_t)plv;
    /* printf("%x\n", res); */
    return res;
}

uint64_t r_trap_era() {
    return get_trapframe()->era;
}
void w_trap_era(uint64_t era) {
    get_trapframe()->era = era;
}

void trap_handler() {
    // printf("trap triggered\n");
    uint32_t estat = r_csr_estat();
    uint32_t ecfg = r_csr_ecfg();
    uint64_t era = r_trap_era();
    uint64_t badv = r_csr_badv();

    if (estat & ecfg & HWI_VEC) {
        // this is a hardware interrupt, via IOCR.
        panic("not finish\n");
    } else if (estat & ecfg & TI_VEC) {
        // timer interrupt
        timer_handler();
        w_csr_ticlr(r_csr_ticlr() | CSR_TICLR_CLR);
    } else {
        era += 4;
        w_trap_era(era);
        struct task_struct *p = my_proc();
        uint8_t ecode = (estat & CSR_ESTAT_ECODE) >> 16;
        uint16_t esubcode = (estat & CSR_ESTAT_ESUBCODE) >> 22;
        if (ecode == E_SYS) {
            // system call
            // intr_on();
            syscall_handler();
        } else if (ecode == E_FPD) {
            printf("ignore: float point exception\n");
        } else if (ecode == E_PIL || ecode == E_PIS || ecode == E_PIF) {
            if ( badv > MAIN_KERNEL_DMW ) {
                printf("%s: page illegal exception\n", p->name);
                printf("badv=%p era=%p\n", badv, era);
                printf("tlbrbadv=%p tlbrera=%p\n", r_csr_tlbrbadv(), r_csr_tlbrera());
            }
            // exit(1);
        } else if (ecode == E_ADE){
            printf("%s: address exception\n", p->name);
            if(esubcode == 1) {
                printf("access memory error\n");
            } else if (esubcode == 0){
                printf("fetch instructions error\n");
            } else {
                panic("esubcode not 1 neither 0");
            }
            printf("badv=%p era=%p\n", badv, era);
            printf("pgd=%p\n", r_csr_pgd());
            exit(1);
        } else if (ecode == E_INE){
            printf("%s: instrcution non-exist exception\n", p->name);
            printf("badv=%p era=%p\n", badv, era);
            printf("badi=%p\n", r_csr_badi());
            exit(1);
        } else if (ecode == E_ALE){
            // printf("%s: address not align exception\n", p->name);
            // printf("badv=%p era=%p\n", badv, era);
            // printf("badi=%p\n", r_csr_badi());
            // printf("tlbrbadv=%p tlbrera=%p\n", r_csr_tlbrbadv(), r_csr_tlbrera());
        } else {
            printf("%s: trapcause %x era=%p\n", p->name, estat, era);
            printf("ecode = %x badv = %x\n", ecode, badv);
            // exit(1);
        }
    }
}

void trap_init() {
    // 配置中断向量和使能
    uint32_t ecfg = (0U << CSR_ECFG_VS_SHIFT)  // 向量偏移为0
                  | HWI_VEC                      // 硬件中断使能
                  | TI_VEC;                       // 定时器中断使能
    w_csr_ecfg(ecfg);
    
    // 设置异常入口地址
    w_csr_eentry((uint64_t)trap_entry);
    
    // 设置TLB重填入口地址
    w_csr_tlbrentry((uint64_t)handle_tlbr);
    
    // 初始化定时器
    timer_init();
}
