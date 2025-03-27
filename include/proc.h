#ifndef __PROC_H__
#define __PROC_H__

#define PROC_MAX 64
#define ARGS_MAX 8
#define ENVS_MAX 16

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "vm.h"
#include "types.h"
#include "kstruct/list.h"

struct context {
    uint64_t ra;
    uint64_t tp;
    uint64_t sp;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
    uint64_t t7;
    uint64_t t8;
    uint64_t r21;
    uint64_t fp;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
};

struct trapframe {
    uint64_t ra;
    uint64_t tp;
    uint64_t sp;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
    uint64_t t7;
    uint64_t t8;
    uint64_t r21;
    uint64_t fp;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;

    /* 248 */ 
    uint64_t prmd;
    uint64_t era;
    
};

enum alloc_state {
    UNUSED = 0,
    USED
};

enum proc_state {
    TASK_RUNNING = 0,
    TASK_RUNABLE,
    TASK_ZOMBIE,
    TASK_KILLED,
};

struct task_struct {
    int pid; // 进程编号
    struct task_struct *parent; //父进程
    struct list_head children;  //子进程链表
    // struct list_head sibling;  //兄弟进程链表
    enum proc_state state; // 进程状态
    enum alloc_state allocs; // 进程分配状态
    struct context *context; // 
    struct trapframe *trapframe;
    struct fdtable * fdtable; // 文件描述符表
    uint64_t bss; // bss 段长度
    char name[32]; // 进程名
    char cwd[256]; // 进程目录
    pagetable_t pgtbl; // 进程页表
    void *mapping_list; // 进程映射列表
    void (*callback)(int); // 进程 signal 回调函数
    uint64_t timer_call_time; // SIGALRM 定时器
    struct list_head tasks;
	int exit_code; 				// the exit code of this process
};

extern struct task_struct *cur_proc;

// proc.c
int in_kernel_memory(uintptr_t start, uintptr_t end);
struct task_struct *my_proc();
struct trapframe *get_trapframe();
void yield();
// int kill(struct proc *p);
int exit(int);
void swtch(struct context *old, struct context *new);
void switch_to(struct task_struct *, struct task_struct *);
extern struct task_struct proc[PROC_MAX];
void kernel_proc_init();
int get_proc_id();
int set_proc_id(int pid);
char* getcwd();
int mkdir(int dirfd, const char *pathname, int mode);
int chdir(char *);
struct task_struct *proc_alloc(void);
void proc_free(struct task_struct *proc);
void proc_print();
struct fdtable * fdtable_alloc();

void proc_init();
void scheduler();
// int kill(struct proc *p);
struct task_struct *proc_find(int pid) ;
int kill(struct task_struct *p) ;
int fork_copy_fds(struct task_struct *child, struct task_struct *parent) ;
bool is_valid_fd(struct task_struct * proc, int fd);



#endif
