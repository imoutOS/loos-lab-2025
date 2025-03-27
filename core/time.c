#include "time.h"
#include "proc.h"
#include "klib.h"
#include "asm/cpu.h"
#include "signal.h"


static uint64_t ticks = 0;  // 记录系统启动后的tick数



// 获取当前tick数
uint64_t get_ticks(void) {
    return ticks;
}

// 时钟中断处理函数
void timer_handler(void) {
    ticks++;
    struct task_struct *p = my_proc();
    if (p->callback && (ticks >= p->timer_call_time)) {
        p->timer_call_time = -1;
        do_signal(SIGALRM);
    }
    yield();
}

// 初始化定时器
void timer_init(void) {
    uint64_t tcfg = TIMER_INITVAL | CSR_TCFG_EN | CSR_TCFG_PER;
    w_csr_tcfg(tcfg);
    ticks = 0;
}


