#include "klib.h"
#include "vm.h"
#include "asm/cpu.h"
#include "memlayout.h"


void shutdown() {
    outw(POWEROFF_ADDR, POWEROFF_MASK);
}
