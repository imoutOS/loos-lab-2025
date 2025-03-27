#include "klib.h"
#include "proc.h"
#include "memlayout.h"
#include "mm.h"
#include "fs.h"
#include "asm/mmu.h"
#include "asm/tlb.h"
#include "struct/mlist.h"
#include <elf.h>
#include "kstruct/list.h"

void handle_tlbr();
extern char _kernel_start[], _kernel_end[];
struct task_struct proc[PROC_MAX];

int last_pid = 0;

char* getcwd(char* buf, size_t size) {
    struct task_struct *p = my_proc();
    assert(p);
    strncpy(buf, p->cwd, size);
    return buf;
}
int get_proc_id() {
    struct task_struct *p = my_proc();
    assert(p);
    return p->pid;
}
int set_proc_id(int pid) {
    struct task_struct *p = my_proc();
    assert(p);
    p->pid = pid;
    return 0;
}
int chdir(char *path) {
    // struct proc *p = my_proc();
    // assert(p);
    // tbd
    // argstr(0, p->cwd, sizeof(p->cwd));
    return 0;
}

struct task_struct *proc_find(int pid) {
    for (int i=0; i < PROC_MAX; i++) {
        if ((proc[i].allocs == USED) && (proc[i].pid == pid)) {
            return &proc[i];
        }
    }
    return NULL;
}

struct task_struct *cur_proc = NULL;
struct task_struct *my_proc() {
    if(cur_proc == NULL) panic("my proc");
    return cur_proc;
}

void proc_init() {
    for (int i=0; i < PROC_MAX; i++) {
        memset(&proc[i], 0, sizeof(struct task_struct));
        proc[i].allocs = UNUSED;
        strcpy(proc[i].cwd, "/");
    }
    kernel_proc_init();
    cur_proc = &proc[0];
    assert(last_pid == 1);
}

void kernel_proc_init() {
    struct task_struct *kproc = proc_alloc();
    strcpy(kproc->name, "k");
    kproc->pid = 0;
    init_list_head(&(kproc->children));
    init_list_head(&(kproc->tasks));
    kproc->state = TASK_RUNNING;
    kproc->context = kmalloc(sizeof(struct context));
    kproc->fdtable = fdtable_alloc();
    // printf("allcoed: fdtable %p\n", kproc->fdtable);
}

struct task_struct *proc_alloc() {
    int i;
    for (i=0; i < PROC_MAX; i++) {
        if (proc[i].allocs == UNUSED)
            goto found;
    }
    panic("proc");
    return NULL;

found:
    proc[i].context = NULL;
    proc[i].trapframe = NULL;
    proc[i].fdtable = fdtable_alloc();
    proc[i].pid = last_pid++;
    proc[i].allocs = USED;
    proc[i].bss = 0;
    proc[i].pgtbl = pgtbl_create();
    proc[i].mapping_list = mlist_init();
    proc[i].callback = NULL;
    proc[i].timer_call_time = -1;
	init_list_head(&proc[i].children);
	init_list_head(&proc[i].tasks);
    // printf("Returning %p for proc[%d]\n", &proc[i], i);
    return &proc[i];
}

void proc_free(struct task_struct *proc) {
    pgtbl_free(proc);
    free(proc->mapping_list);
    free(proc->context);
    free(proc->trapframe); 
    free(proc->fdtable);
    list_del(&proc->children);
    list_del(&proc->tasks);
    proc->allocs = UNUSED;
}

void proc_print() {
    for (int i=0; i < PROC_MAX; i++) {
        if (proc[i].allocs != UNUSED) {
            char *state = (proc[i].state == TASK_RUNABLE) ? "runnable" :
                          (proc[i].state == TASK_RUNNING) ? "running" :
                          (proc[i].state == TASK_ZOMBIE) ? "zombie" :
                          (proc[i].state == TASK_KILLED) ? "killed" : "unknown";
            printf("%s: pid %d ppid %d state %s\n", proc[i].name, proc[i].pid, proc[i].parent->pid, state);
        }
    }
}

void yield() {
    struct task_struct *p = my_proc();
    struct task_struct *kproc = &proc[0];
    if (p->state == TASK_RUNNING) {
        p->state = TASK_RUNABLE;
    }
    kproc->state = TASK_RUNNING;
    // Log("yield: %s\n", p->name);
    swtch(p->context, kproc->context);
}

int kill(struct task_struct *p) {
    p->state = TASK_ZOMBIE;
    if (p == my_proc()) {
        yield();
    }
    return -1;
}

int exit(int _) {
    struct task_struct *p = my_proc();
    Log("process %s exit\n", p->name);
	p->exit_code = _;
    kill(p);
    return -1; // does not return value
}

void switch_to(struct task_struct * before, struct task_struct *after) {

    cur_proc = after;
    w_csr_pgdl((uint64_t)after->pgtbl);
    invtlb_all();

    swtch(before->context, after->context);
    invtlb_all();
}

struct fdtable * fdtable_alloc(){
    struct fdtable * result = kmalloc(PGSIZE);
    memset(result, 0, PGSIZE);
    result[STDIN].fds_index = STDIN;
    result[STDIN].busy = 1;
    result[STDOUT].fds_index = STDOUT;
    result[STDOUT].busy = 1;
    result[STDERR].fds_index = STDERR;
    result[STDERR].busy = 1;
    return result;
}

struct fdtable * fdtable_copy(struct fdtable * fdt){
    struct fdtable * result = kmalloc(PGSIZE);
    for ( int i = 0 ; i < MAX_FDTABLE ; i ++ ){
        if ( fdt[i].busy == 1 ) fds[fdt[i].fds_index].refcount += 1;
    }
    memcpy(result, fdt, PGSIZE);
    return result;
}
