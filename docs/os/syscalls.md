# 初赛赛题分析

## pipe

read, write, close

clone, exit, wait4, pipe

## dup2

```bash
4561 write(1,0x2578,17)========== START  = 17
4561 write(1,0x25e8,9)test_dup2 = 9
4561 write(1,0x2590,12) ==========
 = 12
4561 dup3(1,100,0) = 100
4561 write(100,0x25c0,14)  from fd 100
 = 14
4561 write(1,0x25d0,15)========== END  = 15
4561 write(1,0x25e8,9)test_dup2 = 9
4561 write(1,0x2590,12) ==========
 = 12
4561 exit(0)
```

## clone

```bash
5628 write(1,0x2608,17)========== START  = 17
5628 write(1,0x46b0,10)test_clone = 10
5628 write(1,0x2620,12) ==========
 = 12
5628 clone(0x11,child_stack=0x0000000000002a98,parent_tidptr=0x0000000000000000,tls=0x0000000000000000,child_tidptr=0x0000000000000000) = 5643
5628 wait4(-1,0x75c1417fffcc,0,(nil)) = 0
5643 write(1,0x25e8,27)  Child says successfully!
 = 27
5643 exit(0)
 = 5643
5628 write(1,0x2650,32)clone process successfully.
pid: = 32
5628 write(1,0x2670,0) = 0
5628 write(1,0x417ffef4,4)5643 = 4
5628 write(1,0x2672,1)
 = 1
5628 write(1,0x2690,15)========== END  = 15
5628 write(1,0x46b0,10)test_clone = 10
5628 write(1,0x2620,12) ==========
 = 12
5628 exit(0)
```

## wait

```bash
7829 write(1,0x25b8,17)========== START  = 17
7829 write(1,0x2670,9)test_wait = 9
7829 write(1,0x25d0,12) ==========
 = 12
7829 clone(0x11,child_stack=0x0000000000000000,parent_tidptr=0x000000000000000c,tls=0x0000000000000000,child_tidptr=0x0000000000000000) = 7831
7829 wait4(-1,0x74169e7fffbc,0,(nil)) = 0
7831 write(1,0x25e0,22)This is child process
 = 22
7831 exit(0)
 = 7831
7829 write(1,0x2618,29)wait child success.
wstatus:  = 29
7829 write(1,0x2635,0) = 0
7829 write(1,0x9e7ffee7,1)0 = 1
7829 write(1,0x2637,1)
 = 1
7829 write(1,0x2658,15)========== END  = 15
7829 write(1,0x2670,9)test_wait = 9
7829 write(1,0x25d0,12) ==========
 = 12
7829 exit(0)
```

## exit

```bash
7634 write(1,0x2590,17)========== START  = 17
7634 write(1,0x2610,9)test_exit = 9
7634 write(1,0x25a8,12) ==========
 = 12
7634 clone(0x11,child_stack=0x0000000000000000,parent_tidptr=0x000000000000000c,tls=0x0000000000000000,child_tidptr=0x0000000000000000) = 7636
7634 wait4(-1,0x72b1039fffcc,0,(nil)) = 0
7636 exit(0)
 = 7636
7634 write(1,0x25d8,9)exit OK.
 = 9
7634 write(1,0x25f8,15)========== END  = 15
7634 write(1,0x2610,9)test_exit = 9
7634 write(1,0x25a8,12) ==========
 = 12
7634 exit(0)
```

## waitpid

```bash
8251 write(1,0x25f8,17)========== START  = 17
8251 write(1,0x26b0,12)test_waitpid = 12
8251 write(1,0x2610,12) ==========
 = 12
8251 clone(0x11,child_stack=0x0000000000000000,parent_tidptr=0x000000000000000c,tls=0x0000000000000000,child_tidptr=0x0000000000000000) = 8253
8251 wait4(8253,0x786ec8ffffac,0,(nil)) = 0
8253 sched_yield(0,0,12,0,0,0) = 0
8253 write(1,0x2640,22)This is child process
 = 22
8253 exit(3)
 = 8253
8251 write(1,0x2658,31)waitpid successfully.
wstatus:  = 31
8251 write(1,0x2677,0) = 0
8251 write(1,0xc8fffed7,1)3 = 1
8251 write(1,0x2679,1)
 = 1
8251 write(1,0x2690,15)========== END  = 15
8251 write(1,0x26b0,12)test_waitpid = 12
8251 write(1,0x2610,12) ==========
 = 12
8251 exit(0)
```

## fork

```bash
8563 write(1,0x2588,17)========== START  = 17
8563 write(1,0x2620,9)test_fork = 9
8563 write(1,0x25a0,12) ==========
 = 12
8563 clone(0x11,child_stack=0x0000000000000000,parent_tidptr=0x000000000000000c,tls=0x0000000000000000,child_tidptr=0x0000000000000000) = 8565
8563 wait4(-1,0x7f33a3dfffcc,0,(nil)) = 0
8565 write(1,0x25f0,17)  child process.
 = 17
8565 exit(0)
 = 8565
8563 write(1,0x25d0,26)  parent process. wstatus: = 26
8563 write(1,0x25ea,0) = 0
8563 write(1,0xa3dffef7,1)0 = 1
8563 write(1,0x25ec,1)
 = 1
8563 write(1,0x2608,15)========== END  = 15
8563 write(1,0x2620,9)test_fork = 9
8563 write(1,0x25a0,12) ==========
 = 12
8563 exit(0)
```

## yield

sched_yield, wait4, clone, getpid

## execve

easy, 

## mkdir, getcwd, unlink, fstat, chdir, 

## munmap, mmap, gettimeofday
