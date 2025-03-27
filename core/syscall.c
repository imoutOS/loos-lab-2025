#include "klib.h"
#include "proc.h"
#include "fs.h"
#include "syscall.h"
#include "mm.h"
#include "exec.h"
#include "asm/mmu.h"
#include "kstruct/list.h"
#include "time.h"

uint64_t cur_sp;

struct trapframe *get_trapframe() {
    return (void*)cur_sp;
}

static uint64_t argraw(int n) {
    struct trapframe *trapframe = get_trapframe();
    switch (n) {
        case 0: return trapframe->a0;
        case 1: return trapframe->a1;
        case 2: return trapframe->a2;
        case 3: return trapframe->a3;
        case 4: return trapframe->a4;
        case 5: return trapframe->a5;
    }
    panic("called argraw %d", n);
    return -1;
}

int argint(int n, uint64_t *ip) {
    *ip = argraw(n);
    return 0;
}

int argaddr(int n, uint64_t *ip) {
    *ip = argraw(n);
    return 0;
}

int argstr(int n, char *buf, int max) {
    uint64_t addr;
    if(argaddr(n, &addr) < 0)
        return -1;  
    memcpy(buf, (void*)addr, max);
    return 0;
}

void get_path(struct task_struct *p, char *buf, char *ref_path) {
    if (*(uint8_t*)(ref_path) != '/') {
        strcpy(buf, p->cwd);
        if (strcmp(p->cwd, "/") != 0)
            strcat(buf, "/");
        strcat(buf, ref_path);
    } else {
        strcpy(buf, ref_path);
    }
}

uint64_t sys_openat(void) {
    struct task_struct *p = my_proc();
    char path[256] = {'\0'};
    int dirfd = argraw(0);
    // argstr(1, path, 128);
    int flags = argraw(2);
    get_path(p, path, (char*)argraw(1));
    return openat(dirfd, path, flags);
}

uint64_t sys_close(void) {
    int fd = argraw(0);
    return close(fd);
}

uint64_t sys_read(void) {
    int fd = argraw(0);
    uint64_t buf;
    argaddr(1, &buf);
    size_t count = argraw(2);
    return read(fd, (void*)buf, count);
}

uint64_t sys_write(void) {
    int fd = argraw(0);
    uint64_t buf;
    argaddr(1, &buf);
    size_t count = argraw(2);
    return write(fd, (void*)buf, count);
}

uint64_t sys_lseek(void) {
    int fd = argraw(0);
    int offset = argraw(1);
    int whence = argraw(2);
    return lseek(fd, offset, whence);
}

uint64_t sys_fcntl() {
    int fd = argraw(0);
    int cmd = argraw(1);
    int arg = argraw(2);
    return fcntl(fd, cmd, arg);
}

uint64_t sys_brk() {
    struct task_struct *p = my_proc();
    uint64_t addr = argraw(0);
    assert(p->bss);
    if (addr) {
        if (addr > p->bss) {
            // alloc_vm(p, p->bss, addr, PTE_P|PTE_W|PTE_PLV|PTE_MATL|PTE_D|PTE_V);
        }
        // FIXME: uvmalloc not work
        //        uvmalloc may replace exist vm
        //        uvmdealloc
        p->bss = addr;
    }
    return p->bss;
}

uint64_t sys_mmap() {
    uint64_t addr = argraw(0);
    uint64_t length = argraw(1);
    // uint64_t prot = argraw(2);
    // uint64_t flags = argraw(3);
    uint64_t fd = argraw(4);
    // uint64_t offset = argraw(5);

    if (fd != -1) {
        // panic("mmap fd not support");
    }
    if (addr) {
        return addr;
    }
    // gradually leak memory
    // printf("mmap: length %d\n", length);
    return (uint64_t)kmalloc(length);
}

struct iovec {
    void *iov_base;
    size_t iov_len;
};
uint64_t sys_writev() {
    int fd = argraw(0);
    struct iovec *iov = (void*)argraw(1);
    size_t len = argraw(2);
    for (size_t i=0; i < len; i++) {
        // Log("writev: fd: %d, %p, len %d\n", fd, iov[i].iov_base, iov[i].iov_len);
        write(fd, iov[i].iov_base, iov[i].iov_len);
    }
    return 0;
}

uint64_t sys_clone() {
    struct task_struct *p = my_proc();
    struct task_struct *np = fork(p);
    return np->pid;
}

uint64_t sys_execve() {
    struct task_struct *p = my_proc();
    uint64_t filename_addr = argraw(0),
        *argv_addr = (uint64_t*)argraw(1),
        *envp_addr = (uint64_t*)argraw(2);

    char buf[256] = {'\0'};
    char *argv = (char*)argv_addr;
    char *envp = (char*)envp_addr;
    get_path(p, buf, (char*)filename_addr);

    struct task_struct *np = exec(buf, (char**)argv, (char**)envp);
    if ( !np) {
        return -1;
    }
    
    strcpy(np->name, buf);
    strcpy(np->cwd, p->cwd);
    np->pid = p->pid;
    np->parent = p->parent;
    list_add(&((p->parent)->children), &np->tasks);
    p->pid = -1;
    p->state = TASK_KILLED; 

    return 0;
}

#include <sys/utsname.h>
uint64_t sys_newuname() {
    struct utsname *newuname = (void*)argraw(0);
    strcpy(newuname->sysname, "LoOS");
    strcpy(newuname->nodename, "localhost");
    strcpy(newuname->release, "v1.2");
    strcpy(newuname->version, "Shiro しろ");
    strcpy(newuname->machine, "LoongArch64");
    return 0;
}

uint64_t sys_read_link_at() {
    // int dirfd = argraw(0);
    // char *pathname = (void*)argraw(1);
    // char *buf = (void*)argraw(2);
    // size_t bufsize = argraw(3);

    // Log("readlinkat: %s\n", pathname);
    // assert(strcmp(pathname, "/proc/self/exe") == 0);
    return 0;
}

uint64_t sys_getrandom() {
    uint8_t *buf = (void*)argraw(0);
    size_t bufsize = argraw(1);
    // uint64_t flags = argraw(2);
    for (size_t i=0; i < bufsize; i++) {
        buf[i] = (uint8_t)rand();
    }
    return bufsize;
}

// #include "struct/time.h"
uint64_t tick = 0;

void do_signal(uint64_t signum) {
    struct task_struct *p = my_proc();
    if ( !p->callback) {
        panic("%s do_signal", p->name);
    }
    // FIXME: callback with cpl=0
    p->callback(signum);
}

#include "signal.h"
// conflicting types for 'kill'
uint64_t sys_rt_sigaction() {
    struct task_struct *p = my_proc();
    // uint64_t signum = argraw(0);
    struct sigaction *act = (void*)argraw(1);
    // struct sigaction *old = (void*)argraw(2);

    /*printf("sa_handler %x\n", act->sa_handler);
    printf("sa_sigaction %x\n", act->sa_sigaction);
    printf("sa_mask %x\n", act->sa_mask);
    printf("sa_flags %x\n", act->sa_flags);
    printf("sa_restorer %x\n", act->sa_restorer);*/

    assert(act->sa_handler);
    p->callback = act->sa_handler;
    return 0;
}

uint64_t get_ns() {
    return tick * 1000 * 2;
}
uint64_t get_tick_by_ms(uint64_t ms) {
    // just for test
    return ms / 2;
}
uint64_t sys_setitimer() { // TODO
    struct task_struct *p = my_proc();
    // int which = argraw(0);
    struct itimerval *new = (void*)argraw(1);
    // struct itimerval *old = (void*)argraw(2);

    assert(new->it_interval.tv_sec == 0 && new->it_interval.tv_usec == 0);
    p->timer_call_time = tick + get_tick_by_ms(((uint64_t)new->it_value.tv_sec)*10 + new->it_value.tv_usec);
    return 0;
}

void clean_up() {
    struct task_struct *p = my_proc();
    p->callback = NULL;
    p->timer_call_time = -1;
}

uint64_t sys_exit() {
    int ret = argraw(0);
    clean_up();
    exit(ret);
    return -1;
}

uint64_t sys_getcwd() {
    char* buf = (void*)argraw(0);
    size_t size = argraw(1);
    return (uint64_t)getcwd(buf, size);
}

uint64_t sys_chdir() {
    struct task_struct *p = my_proc();
    char path[256], newpath[256];
    argstr(0, path, 256);
    if (strcmp(path, "..") == 0) {
        strcpy(newpath, p->cwd);
        int i = 0;
        for (i = strlen(p->cwd); i > 0; i--) {
            if (newpath[i] == '/') {
                newpath[i] = '\0';
                break;
            }
        }
        if (i == 0)
            strcpy(newpath, "/");
    } else if (path[0] == '/') {
        strcpy(newpath, path);
    } else {
        strcpy(newpath, p->cwd);
        if (strcmp(p->cwd, "/") != 0)
            strcat(newpath, "/");
        strcat(newpath, path);
    }
    strcpy(p->cwd, newpath);
    // return chdir(path);
    printf("chdir %s\n", p->cwd);
    return 0;
}

uint64_t sys_get_proc_id() {
    return get_proc_id();
}

uint64_t sys_set_proc_id() {
    int newpid = argraw(0);
    return set_proc_id(newpid);
}

uint64_t sys_dup3() {
    int oldfd = argraw(0);
    int newfd = argraw(1);
    int flags = argraw(2);
    return dup3(oldfd, newfd, flags);
}

uint64_t sys_dup2() {
    int oldfd = argraw(0);
    int newfd = argraw(1);
    return dup3(oldfd, newfd, 0);
}

uint64_t sys_dup() {
    int fd = argraw(0);
    return dup(fd);
}

#include <poll.h>
uint64_t sys_poll() {
    // struct proc *p = my_proc();
    // uint64_t fds = argraw(0),
        // nfds = argraw(1),
        // timeout = argraw(2);
    return POLLIN;
}

#define __timeval_defined 1
#include <bits/types/struct_rusage.h>
uint64_t sys_wait4() {
    // proc_print();
    uint64_t pid = argraw(0),
        wstatus = argraw(1),
        // options = argraw(2),
        rusage = argraw(3);

    struct task_struct *p = my_proc();
    struct task_struct *pos = NULL;
    if (pid == -1){
        cycle_start:
        bool runnings = 0;
        list_for_each_entry(pos, &(p->children), tasks)
        {
            if( pos->state == TASK_ZOMBIE )
            {
                goto find_proc_sat;
                break;
            }
            runnings = runnings | (pos->state == TASK_RUNNING || pos->state == TASK_RUNABLE);
        }
        if ( ! runnings ) return -1;
        yield();
        goto cycle_start;
    } else {
        pos = proc_find(pid);
        if (pos && pos->parent != p) return -1;
        while(pos->state != TASK_ZOMBIE){
            yield();
        }
    }
    // hack
    find_proc_sat:
    Log("pos->pid = %d\n", pos->pid);
    if (rusage) {
        memset((void*)rusage, 0, sizeof(struct rusage));
        struct rusage *r = (void*)rusage;
        r->ru_stime.tv_sec = 1;
        r->ru_utime.tv_sec = 1;
    }
    if (wstatus) {
        *(uint64_t *) wstatus = pos->exit_code << 8;
    }
    pos->state = TASK_KILLED;
    return pos->pid;
}

uint64_t sys_pipe2() {
    panic("pipe machanism not implement");
}

uint64_t sys_sendfile() {
    uint64_t out_fd = argraw(0),
        in_fd = argraw(1),
        offset = argraw(2);
        // count = argraw(3);
    // printf("sendfile in %d out %d\n", in_fd, out_fd);
    if (offset)
        panic("sendfile offset");
    uint64_t total_len = 0, this_len;
    char buf[2048];
    while (1) {
        this_len = read(in_fd, buf, 1);
        if ( !this_len)
            break;
        write(out_fd, buf, this_len);
        total_len += this_len;
    }
    return total_len;
}
 


uint64_t sys_return_zero() {
    return 0;
}
uint64_t sys_return_wrong() {
    return -1;
}
uint64_t sys_return_random() {
    return rand();
}

static uint64_t (*syscalls[])(void) = {
    [SYS_openat]    sys_openat,
    [SYS_close]     sys_close,
    [SYS_read]      sys_read,
    [SYS_write]     sys_write,
    [SYS_lseek]     sys_lseek,
    [SYS_exit]      sys_exit,
    [SYS_fstat]     sys_return_wrong,
    [SYS_fcntl]     sys_fcntl,
    [SYS_exit_group]    sys_exit,
    [SYS_brk]       sys_brk,
    [SYS_mmap]      sys_mmap,
    [SYS_writev]    sys_writev,
    [SYS_newuname]  sys_newuname,
    [SYS_readlinkat]    sys_read_link_at,
    [SYS_getrandom] sys_getrandom,
    [SYS_rt_sigaction]  sys_rt_sigaction,
    [SYS_setitimer] sys_setitimer,
    [SYS_clone]     sys_clone,
    [SYS_wait4]     sys_wait4,
    [SYS_execve]    sys_execve,

    [SYS_set_tid_address]   sys_get_proc_id,
    [SYS_set_robust_list]   sys_return_zero,
    [SYS_rt_sigprocmask]    sys_return_zero,
    [SYS_gettid]            sys_get_proc_id,
    [SYS_getpid]            sys_get_proc_id,
    [SYS_getppid]           sys_get_proc_id,

    [SYS_geteuid]           sys_return_zero,
    [SYS_getuid]            sys_return_zero,
    [SYS_getgid]            sys_return_zero,
    [SYS_getpgid]           sys_return_zero,

    [SYS_setuid]            sys_return_zero,
    [SYS_setpgid]           sys_return_zero,
    [SYS_setgid]            sys_return_zero,

    [SYS_tgkill]            sys_return_zero,
    [SYS_prlimit64]         sys_return_zero,
    [SYS_mprotect]          sys_return_zero,
    [SYS_statx]             sys_return_zero,

    [SYS_chdir]             sys_chdir,
    [SYS_getcwd]            sys_getcwd,
    [SYS_dup3]              sys_dup3,
    [SYS_getdents64]        sys_return_zero,
    [SYS_mkdirat]           sys_return_wrong,
    [SYS_dup]               sys_dup,
    [SYS_pipe2]             sys_pipe2,
    [SYS_unlinkat]          sys_return_wrong,

    [SYS_ioctl]             sys_return_zero,
    [SYS_ppoll]             sys_poll,
    [SYS_reboot]            sys_return_zero,
    [SYS_nanosleep]         sys_return_zero,
    [SYS_munmap]            sys_return_zero,
    [SYS_clock_gettime]     sys_return_zero,
    [SYS_gettimeofday]      sys_return_zero,
    [SYS_faccessat]         sys_return_wrong,
    [SYS_pipe2]             sys_pipe2,
    [SYS_sendfile]          sys_sendfile,

};


void syscall_handler() {

    struct trapframe *trapframe = get_trapframe();
    int num = trapframe->a7;

    uint64_t ret = -1;
    uint64_t (*syscall)() = NULL;
    if ((num >= sizeof(syscalls) / sizeof(syscalls[0])) || ((syscall = syscalls[num]) == NULL)) {
        printf("syscall id: %d\n", num);
        printf("id: %d, arg1: %x, arg2: %x, arg3: %x\n", num, argraw(0), argraw(1), argraw(2));
        // panic("syscall not implemented");
        printf("syscall not implemented\n");
        syscall = syscalls[SYS_exit_group];
    }
    ret = syscall();
    trapframe->a0 = ret;

}
