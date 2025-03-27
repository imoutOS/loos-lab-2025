#include "fs/pipe.h"
#include "klib.h"
#include "ext2.h"
#include "fs.h"
#include "proc.h"
#include "errno.h"
#include "fs/fsio.h"
#include "types.h"
#include "fs/fcntl.h"


size_t read(int fd, void *buf, size_t count) {
    if (!is_valid_fd(my_proc(), fd)) {
        return -EBADF;
    }
    struct ext2_file * f = my_proc()->fdtable->entries[fd].file;
    if (f->f_flags & TEST_FLAGS_STDIN)
    {
        char *str = getstr(count);
        count = strlen(str);
        memcpy(buf, str, count);
        return count;
    }

    uint32_t size = 0;
    /* ext4_fread(&(ftf(fd)->f), buf, count, &size); */
    // Log("read fd %d, size %d\n", fd, size);
    ext2_fread((f->f_inode), buf, count, &size); // TODO
    return size;
}

size_t write(int fd, const void *buf, size_t count) {
    Log("writing fd %d buf %p count %d\n", fd, buf, count);
    // struct proc * proc = my_proc();
    if (!is_valid_fd(my_proc(), fd)) {
        return -1;
    }

    struct ext2_file * f = my_proc()->fdtable->entries[fd].file;

    if((f->f_flags & TEST_FLAGS_STDOUT) || (f->f_flags & TEST_FLAGS_STDERR)){
        for (int i = 0; i < count; i++) {
            putch(((char *)buf)[i]);
        }
        return count;
    }


    return 0;
}

int creat(const char *path, mode_t mode) {
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}
