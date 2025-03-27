#include "klib.h"
#include "mm.h"
#include "vm.h"
#include "asm/cpu.h"
#include "fs.h"
#include "proc.h"
#include "driver.h"
#include "exec.h"
#include "trap.h"
#include "kstruct/list.h"

int kmain() {
    w_csr_euen(1);
    // dump_dmw();
    dmw_init();
    // kinit();
    trap_init();
    tlb_init();
    vm_init();
    pmm_init();
    fs_init();
    proc_init();
    disk_init();
    user_init();
    intr_on();  
    scheduler();

    // printf("kernel: finish\n");
    while (1);
    panic("should not reach here");
}
