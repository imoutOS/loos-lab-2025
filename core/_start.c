#include "asm/cpu.h"
#include "klib.h"

extern int kmain();
extern char _stack_pointer;
extern char _bss_start, _bss_end;
void _start() __attribute__ ((section ("._start")));

void _start()  {
    void *ksp = &_stack_pointer; // - 0x20;
    w_sp((uint64_t)ksp);
    intr_off();
    memset(&_bss_start, 0, (uint64_t)&_bss_end-(uint64_t)&_bss_start);
    kmain();
    while (1);
}
