#ifndef FS_H
#define FS_H
#include "klib.h"

#include "ext2.h"
#include "types.h"
#include "ahci.h"

#define MAX_FILES 256

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7

#define O_CLOEXEC 02000000
#define O_CREAT 0100
#define FD_CLOEXEC 1

// dirent.h
#define DT_UNKNOWN  0 // Unknown file type
#define DT_FIFO     1 // Named pipe (FIFO)
#define DT_CHR      2 // Character device
#define DT_DIR      4 // Directory
#define DT_BLK      6 // Block device
#define DT_REG      8 // Regular file
#define DT_LNK     10 // Symbolic link
#define DT_SOCK    12 // UNIX domain socket
#define DT_WHT     14 // Whiteout

typedef unsigned long int dev_t;
typedef unsigned long int ino_t;
typedef unsigned int mode_t;
typedef unsigned int nlink_t;
// typedef unsigned int uid_t;
// typedef unsigned int gid_t;
typedef long int off_t;
typedef long int blksize_t;
typedef long int blkcnt_t;
typedef long int time_t;

#define FILE_STDIN_FLAGS   0x1
#define FILE_STDOUT_FLAGS  0x2
#define FILE_STDERR_FLAGS  0x4
#define FILE_PIPE_FLAGS    0x8
#define FILE_PIPE_R_FLAGS  0x10
#define FILE_PIPE_W_FLAGS  0x20

#define MAX_FDTABLE 256
struct fdtable {
    int fds_index;
    int busy;
};


int openat(int dirfd, const char *pathname, int flags);

int close(int fd);

int lseek(int fd, int offset, int whence);

size_t read(int fd, void *buf, size_t count);

size_t write(int fd, const void *buf, size_t count);

int dup3(int oldfd, int newfd, int flags);

int fcntl(int fd, int cmd, ...);
int unlink(const char *pathname);
int dup(int oldfd);
int dup3(int oldfd, int newfd, int flags);
int find_inode_by_pathname(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);

void fs_init();
void find_and_exec(void);

int close_fd(int fd);

// file
struct ext2_file {
    struct ext2_inode * f_inode;
    uint32_t f_pos;
    uint32_t f_flags;
    int refcount;
    int alloc;
};


extern struct ext2_file fds[MAX_FILES];


#endif // FS_H
