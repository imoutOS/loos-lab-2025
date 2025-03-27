#include "klib.h"
#include "disk.h"
#include "ext2.h"
#include "fs.h"
#include "proc.h"
#include "mm.h"
#include "errno.h"


// #define MAX_FILES 1024
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define ftf(X) fd_to_fds(X)

struct ext2_file fds[MAX_FILES];

int inode_cnt = 0;

// 初始化文件描述符表
void fs_init() {
    memset(fds, 0, sizeof(fds));
    fds[STDIN].alloc = 1;
    fds[STDOUT].alloc = 1;
    fds[STDERR].alloc = 1;
    // fds[STDIN].is_open = 1; // 标准输入
    // fds[STDOUT].is_open = 1; // 标准输出
    // fds[STDERR].is_open = 1; // 标准错误
}

char filepath_abs[256];

void dump_fds(){
    for ( int i = 0 ; i < MAX_FILES ; i ++){
        if( fds[i].alloc == 1 ){
            if ( i >= STDIN && i <= STDERR ) continue;
            if ( fds[i].f_inode == NULL) continue;
            /* Log("%d: %s\n", i, name_by_inode[fds[i].f_inode->i_ino]); */
        }
    }
}

struct ext2_file * fd_to_fds(int fd){
    struct task_struct * proc = my_proc();
    if ( proc->fdtable[fd].busy == 0 ) panic("ftf, not busy");
    int index = proc->fdtable[fd].fds_index;
    return &fds[index];
}

int alloc_fds(){
    for ( int i = 0 ; i < MAX_FILES ; i ++ ){
        if ( fds[i].alloc == 0 ) {
            fds[i].alloc = 1;
            Log("allocing fds: %d\n", i);
            return i;
        }
    }
    panic("alloc fds");
}

void free_fds(int index){
    // dump_fds();
    Log("free fds: %d\n", index);
    if ( fds[index].alloc != 1 ) panic("free fds");
    fds[index].alloc = 0;
    // if( fds[index].flags & FILE_PIPE_FLAGS ) {
        //
    // }
}

int open(const char *pathname, int flags, ...) {
    struct ext2_inode inode;
    int ret = ext2_path_lookup(&e2fs, pathname, &inode);
    if(ret != 0) return -1;

    struct task_struct *proc = my_proc();
    for(int i = 0; i < MAX_FDTABLE; i++){
        if(!proc->fdtable[i].busy){
            proc->fdtable[i].busy = 1;
            proc->fdtable[i].fds_index = alloc_fds();

            struct ext2_file *f = &fds[proc->fdtable[i].fds_index];
            f->f_inode = kmalloc(sizeof(struct ext2_inode));
            memcpy(f->f_inode, &inode, sizeof(struct ext2_inode));

            f->f_pos = 0;
            f->f_flags = flags;
            return i;
        }
    }
    return -1;
}

int openat(int dirfd, const char *pathname, int flags) {
    return open(pathname, flags);
}

static inline int fd_invaild(int fd) {
    struct task_struct * proc = my_proc();
    int result = (fd < 0) || (fd >= MAX_FDTABLE) || (proc->fdtable[fd].busy == 0);
    if( result == 1 ) {
        Log("proc name: %s %p\n", proc->name, proc);
        Log("cur fd = %d\n", fd);
        Log("1 : %d 2 : %d 3 : %d\n", (fd < 0) , (fd >= MAX_FDTABLE) , (proc->fdtable[fd].busy == 0));
        for ( int i = 0 ; i < MAX_FDTABLE ; i ++ ) {
            if ( proc->fdtable[i].busy == 1 ) {
                Log("%d: %d\n", i, proc->fdtable[i].fds_index);
            }
        }
    }
    return (fd < 0) || (fd >= MAX_FDTABLE) || (proc->fdtable[fd].busy == 0);
}

int close(int fd) {
    struct task_struct * proc = my_proc();
    /* Log("closing fd: %d(fds %d) proc: %d\n", fd, proc->fdtable[fd].fds_index, proc->pid); */
    // Log("                  proc %d, name %s, father %p\n", proc->pid, proc->name, proc->ppid);
    if (fd_invaild(fd)) {
        return -1;
    }

    ftf(fd)->refcount -= 1;
    if(ftf(fd)->refcount == 0) {
        if ( ftf(fd)->f_flags & FILE_PIPE_FLAGS ){
        } else {
            /* ext4_fclose(&(ftf(fd)->f)); how to replace this? */
            free_fds(proc->fdtable[fd].fds_index);
        }
    }
    proc->fdtable[fd].busy = 0;
    return 0;
}
int close_fd(int fd) {
    struct task_struct * proc = my_proc();
    if (fd_invaild(fd)) {
        return -1;
    }

    ftf(fd)->refcount -= 1;
    if(ftf(fd)->refcount == 0) {
        free_fds(proc->fdtable[fd].fds_index);
    }
    proc->fdtable[fd].busy = 0;
    return 0;
}

int lseek(int fd, int offset, int whence) {
    if (fd_invaild(fd)) {
        return -1;
    }
    return -1;
}

int wait_counts = 0;
#define MAX_WAIT 100

size_t read(int fd, void *buf, size_t count) {
    // struct proc * proc = my_proc();
    Log("reading fd: %d, %p, %d, %d\n", fd, buf, count, proc->pid);
    // Log("reading from proc: %p, fd = %d\n", proc, fd);
    // Log("             proc: %d, name %s, father %p\n", proc->pid, proc->name, proc->ppid);
    if (fd_invaild(fd)) {
        return -1;
    }
    struct ext2_file * f = ftf(fd);

    if (f == &fds[STDIN]) {
        char *str = getstr(count);
        count = strlen(str);
        memcpy(buf, str, count);
        return count;
    }

    uint32_t size = 0;
    /* ext4_fread(&(ftf(fd)->f), buf, count, &size); */
    // Log("read fd %d, size %d\n", fd, size);
    ext2_fread((ftf(fd)->f_inode), buf, count, &size); // TODO
    return size;
}

size_t write(int fd, const void *buf, size_t count) {
    Log("writing fd %d buf %p count %d\n", fd, buf, count);
    // struct proc * proc = my_proc();
    if (fd_invaild(fd)) {
        return -1;
    }

    if(ftf(fd) == &fds[STDOUT] || ftf(fd) == &fds[STDERR]){

    // if ((proc->fdtable[fd].flags & FILE_STDOUT_FLAGS) ||
            // (proc->fdtable[fd].flags & FILE_STDERR_FLAGS) ) {
        for (int i = 0; i < count; i++) {
            putch(((char *)buf)[i]);
        }
        return count;
    }

    return 0;
}


int fcntl(int fd, int cmd, ...) {
    // panic("fcntl");
    struct task_struct * proc = my_proc();
    Log("fcntl: %d, cmd = %d, proc: %d\n", fd, cmd, proc->pid);
    if (fd_invaild(fd)) {
        return -1;
    }

    va_list args;
    va_start(args, cmd);
    int result = -1;

    switch (cmd) {
        case F_DUPFD: {
            int arg = va_arg(args, int);
            for (int i = arg; i < MAX_FILES; i++) {
                if (!proc->fdtable[i].busy) {
                    proc->fdtable[i] = proc->fdtable[fd];
                    result = i;
                    break;
                }
            }
            break;
        }
        case F_GETFD:
            result = proc->fdtable[fd].busy;
            break;
        case F_SETFD: {
            int arg = va_arg(args, int);
            proc->fdtable[fd].busy = arg;
            result = 0;
            break;
        }
        case F_GETFL:
            result = 0;
            break;
        case F_SETFL:
            result = 0;
            break;
        case F_GETLK:
            result = 0;
            break;
        case F_SETLK:
            result = 0;
            break;
        case F_SETLKW:
            result = 0;
            break;
        default:
            result = -1;
            break;
    }

    va_end(args);
    return result;
}

int dup(int oldfd) {
    if ( fd_invaild(oldfd) ) panic("dup");
    struct task_struct * proc = my_proc();
    for ( int i = 0 ; i < MAX_FDTABLE ; i ++){
        if(proc->fdtable[i].busy == 0){
            proc->fdtable[i].fds_index = proc->fdtable[oldfd].fds_index;
            proc->fdtable[i].busy = 1;
            ftf(oldfd)->refcount += 1;
            Log("dup: %d->%d\n", oldfd, i);
            return i;
        }
    }
    return -1;
}

int dup3(int oldfd, int newfd, int flags) {
    struct task_struct * proc = my_proc();
    Log("dup3: %d(fds %d)->%d(fds %d) with flags %d proc: %d\n", oldfd, 
            proc->fdtable[oldfd].fds_index, newfd, 
            proc->fdtable[newfd].fds_index, flags, proc->pid);
    // assert(newfd >= 3 && newfd < MAX_FILES);
    // 检查 oldfd 和 newfd 是否有效
    // if (fd_invaild(oldfd) || fd_invaild(newfd)) {
    //     // errno = EBADF;
    //     return -1;
    // }

    // 如果 oldfd 和 newfd 相同，且没有其他标志，则直接返回 newfd
    if (oldfd == newfd) {
        return newfd;
    }

    if ( fd_invaild(oldfd) ) panic("dup3");

    // 如果 newfd 已经打开，先关闭它
    if (proc->fdtable[newfd].busy == 1) {
        close(newfd);
    }

    // 复制 oldfd 的文件描述符到 newfd
    proc->fdtable[newfd].busy = 1;
    proc->fdtable[newfd].fds_index = proc->fdtable[oldfd].fds_index;
    ftf(oldfd)->refcount += 1;

    // 处理 flags
    if (flags & O_CLOEXEC) {
        // 设置 O_CLOEXEC 标志
        int flags_newfd = fcntl(newfd, F_GETFD);
        if (flags_newfd == -1) {
            return -1;
        }
        if (fcntl(newfd, F_SETFD, flags_newfd | FD_CLOEXEC) == -1) {
            return -1;
        }
    }

    Log("finishing dup3: %d(fds %d)->%d(fds %d) with flags %d proc: %d\n", oldfd, 
            proc->fdtable[oldfd].fds_index, newfd, 
            proc->fdtable[newfd].fds_index, flags, proc->pid);

    // 返回新的文件描述符
    return newfd;
}

int mkdir(int dirfd, const char *pathname, int mode) {
    return 0;
}

int unlinkat(int dirfd, const char *pathname, int flags) {
    return -1; // Directory not found
}
