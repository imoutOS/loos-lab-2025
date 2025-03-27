# 编译子系统 FLAGS 使用指南

## 全局的 FLAGS 修改

全局flags 即为会应用到所有的 .c 文件中的 flags，需要在 topMakefile 文件中修改

在 topMakefile 中有下面三个 FLAG 可以修改

- CFLAGS      :   针对所有 .c 的 flags
- ASFLAGS    :   针对所有 .s 的 flags
- LDFLAGS    :   最后的链接 flags

## 局部的 FLAGS 修改 

某一个目录里面的 Makefile 控制着这一层里面的所有 .s .c 的编译，也可以针对这一个目录里面的 .c .s 使用的 flags

- CFLAGS_EXTRA    :  当前目录中所有 .c 的 flags
- ASFLAGS_EXTRA  :  当前目录中所有 .s 的 flags

## 单一文件的 FLAGS 修改

某一个目录里面的 Makefile 也可以控制这一个目录的某个单一的 .c 或者 .s 文件的 flags

- CFLAG_obj  : 给名称为 obj.c 文件单独添加 CFLAGS
- ASFLAG_obj : 给名称为 obj.s 文件单独添加 ASFLAGS

举例

```
|- Makefile @1
|- ...
|- a
|  |-Makefile @2
|  |- zxc.c
|  |- yyy.c
|  |- b
|     |-Makefile @3
|     |-uuu.c
```



@1 的内容如下

```makefile
obj-y += a/
CFLAGS := -Dglobal
...
...
```

@2 的内容如下

```makefile
obj-y += b/
CFLAGS_EXTRA += -Dtest_a
CFLAG_zxc += -Dtest_zxc
CFLAG_yyy += -Dtest_yyy
obj-y += zxc.o
obj-y += yyy.o
```

@3 的内容如下

```
CFLAGS_EXTRA += -Dtest_b
obj-y += uuu.o
```



对应编译 zxc.c , yyy.c , uuu.c 的flags 为

```
gcc -Dglobal -Dtest_a -Dtest_zxc -c zxc.c -o zxc.o
gcc -Dglobal -Dtest_a -Dtest_yyy -c yyy.c -o yyy.o
gcc -Dglobal -Dtest_b  -c uuu.c -o uuu.o
```

