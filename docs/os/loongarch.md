# LoongArch 相关工具链与评测

## LS2000

> /tmp/qemu/bin/qemu-system-loongarch64 -M ls2k -serial stdio -drive if=pflash,file=/tmp/qemu/2k1000/u-boot-with-spl.bin -m 1024 -vnc :0 -D /tmp/qemu.log -net nic -net user,net=10.0.2.0/24,tftp=/srv/tftp -hda

## SYSCALLS 完善

### SYSCALLS 列表

可以从 [unistd.h](https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/unistd.h) 中获得系统调用的列表，此处需要注意几点：

* 没有 renameat ，请使用 renameat2 。
* 没有 getrlimit 或者 setrlimit ，请使用 prlimit64 。
* 没有 fstat 或者 newfstatat ，只有 statx 。

注：

renameat(38) renameat(276)

getrlimit(163) setrlimit(164) prlimit64(261)

fstat(79) fstatat(80) statx(291)

```c
#if defined(__ARCH_WANT_NEW_STAT) || defined(__ARCH_WANT_STAT64)
#define __NR3264_fstatat 79
__SC_3264(__NR3264_fstatat, sys_fstatat64, sys_newfstatat)
#define __NR3264_fstat 80
__SC_3264(__NR3264_fstat, sys_fstat64, sys_newfstat)
#endif
```

### SYSCALLS 统计

> qemu-loongarch64 -strace -L
> ~/x-tools/loongarch64-unknown-linux-gnu/loongarch64-unknown-linux-gnu/sysroot
> ./busybox sh

有可能出现 `Unknown syscall 79` 这种情况。这是因为（见上），而
[core] 仓库中的 qemu-system 并没有置入对以上数个 syscalls 的模拟，因此
最好使用 ls2k 版本的 qemu 。

> /tmp/qemu/bin/qemu-loongarch64 -strace -L ~/x-tools/loongarch64-unknown-linux-gnu/loongarch64-unknown-linux-gnu/sysroot ./busybox sh 2>syscalls.out.2

接下来可以开始统计：

> cat syscalls.out | awk '{print substr($2,1,index($2,"(")-1)}' | sort | uniq

因此在 busybox 的测试样例中需要的 SYSCALLS 如下：

* ~~brk~~
* clone
* ~~close~~
* ~~dup3~~
* execve
* ~~exit_group~~
* ~~fcntl~~
* ~~fstat~~
* ~~getcwd~~
* ~~geteuid~~
* ~~getgid~~
* ~~getpgid~~
* ~~getpid~~
* ~~getppid~~
* ~~getuid~~
* ioctl
* ~~lseek~~
* ~~openat~~
* pipe2
* ppoll
* ~~read~~
* ~~readlinkat~~
* ~~rt_sigaction~~
* ~~rt_sigprocmask~~
* ~~re_sigreturn~~
* ~~setgid~~
* ~~setpgid~~
* ~~setuid~~
* ~~uname~~
* wait4
* ~~write~~

同理可以测量 `copy-file-range` 测试所需要的 syscalls .

* brk
* copy_file_range
* exit_group
* fstat
* getpid
* gettimeofday
* openat
* readlinkat
* write

同理可以测量 `libc-bench` 测试所需要的 SYSCALLS .

* clone3
* statx

* brk
* clock_gettime
* clone
* close
* exit_group
* futex
* getrandom
* mmap
* mprotect
* openat
* prlimit64
* read
* readlinkat
* rt_sigaction
* rt_sigprocmask
* set_robust_list
* set_tid_address
* wait4
* write

## References

[内核第一条指令（基础篇） - rCore-Tutorial-Book-v3 3.6.0-alpha.1 文档](https://rcore-os.cn/rCore-Tutorial-Book-v3/chapter1/3first-instruction-in-kernel1.html)
