#include "klib.h"
#include "proc.h"
#include "driver.h"

extern struct task_struct proc[PROC_MAX];
// RR
void scheduler() {

    Log("scheduler\n");
    struct task_struct *kproc = &proc[0];
    assert(kproc->pid == 0);
    int run;
    while (1) {
        run = 0;
        for (int i=0; i < PROC_MAX; i++) {
            if ((proc[i].allocs == UNUSED) || (proc[i].pid == 0))
                continue;
            if (proc[i].state == TASK_RUNABLE) {
                run = 1;
                kproc->state = TASK_RUNABLE;
                proc[i].state = TASK_RUNNING;
                switch_to(kproc, &proc[i]);
            }
            else if (proc[i].state == TASK_ZOMBIE) {
            }
            else if (proc[i].state == TASK_KILLED) {
                if (proc[i].pid == 0)
                    panic("Kernel panic: Attempted to kill init!");
            }
        }
        if ( !run) {
            break;
        }
    }
    shutdown();
}
