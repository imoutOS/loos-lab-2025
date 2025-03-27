#ifndef TIME_H
#define TIME_H

#include "types.h"

/* Timer Configuration */
#define CC_FREQ         100000000   // CPU频率：1亿Hz
#define TICK_MS         10          // 每个tick为10ms
#define TIMER_INITVAL   1000000     // 定时器初值：100万

/* Timer CSR Register Bits */
#define CSR_TCFG_EN     (1U << 0)   // 定时器使能位
#define CSR_TCFG_PER    (1U << 1)   // 定时器周期模式位
#define CSR_TICLR_CLR   (1U << 0)   // 定时器中断清除位

typedef long time_t;          // 用于秒数
typedef long suseconds_t;     // 用于微秒数

struct timeval {
    time_t      tv_sec;   // 秒
    suseconds_t tv_usec;  // 微秒
};

struct itimerval {
    struct timeval it_interval;  // 定时器间隔
    struct timeval it_value;     // 定时器当前值
};

struct tm {
    int tm_sec;    // 秒 [0,59]
    int tm_min;    // 分 [0,59]
    int tm_hour;   // 时 [0,23]
    int tm_mday;   // 日 [1,31]
    int tm_mon;    // 月 [0,11]
    int tm_year;   // 年 [从1900年开始]
    int tm_wday;   // 星期几 [0,6] (星期天 = 0)
    int tm_yday;   // 一年中的第几天 [0,365]
    int tm_isdst;  // 夏令时标志
};

// 时钟类型定义
#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC         1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3
#define CLOCK_MONOTONIC_RAW      4
#define CLOCK_REALTIME_COARSE    5
#define CLOCK_MONOTONIC_COARSE   6
#define CLOCK_BOOTTIME          7

typedef int clockid_t;

/* Timer API */
void                        timer_init(void);
void                        timer_handler(void);
uint64_t                    get_ticks(void);

#endif /* !TIME_H */
