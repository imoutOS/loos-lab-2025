#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <stdint.h>
#include <syscall.h>

#define SYSCALL_CLOBBERLIST \
	"$t0", "$t1", "$t2", "$t3", \
	"$t4", "$t5", "$t6", "$t7", "$t8", "memory"

static inline uint64_t _syscall0(uint64_t NUMBER) {
    register uint64_t a0 __asm__("$a0");
	register uint64_t a7 __asm__("$a7") = NUMBER;

	__asm__ __volatile__ (
		"syscall 0"
		: "+&r"(a0)
		: "r"(a7)
		: SYSCALL_CLOBBERLIST);
	return a0;
}

static inline uint64_t _syscall3(uint64_t n, uint64_t a, uint64_t b, uint64_t c) {
	register uint64_t a0 __asm__("$a0") = a;
	register uint64_t a1 __asm__("$a1") = b;
	register uint64_t a2 __asm__("$a2") = c;
	register uint64_t a7 __asm__("$a7") = n;

	__asm__ __volatile__ (
		"syscall 0"
		: "+&r"(a0)
        : "r"(a7), "r"(a1), "r"(a2)
		: SYSCALL_CLOBBERLIST);
	return a0;
}

#define exit(ret) _syscall3(SYS_exit, ret, 0, 0)
#define openat(dirfd, pathname, flags) _syscall3(SYS_openat, dirfd, pathname, flags)
#define close(fd) _syscall3(SYS_close, fd, 0, 0)
#define read(fd, buf, count) _syscall3(SYS_read, fd, buf, count)
#define write(fd, buf, count) _syscall3(SYS_write, fd, buf, count)
#define lseek(fd, offset, whence) _syscall3(SYS_lseek, fd, offset, whence)
#define fstat(pathname, buf) _syscall3(SYS_fstat, pathname, buf, 0)
#define fcntl(fd, cmd, arg) _syscall3(SYS_fcntl, fd, cmd, arg)
#define getdents64(fd, dirp, count) _syscall3(SYS_getdents64, fd, dirp, count)

#endif
