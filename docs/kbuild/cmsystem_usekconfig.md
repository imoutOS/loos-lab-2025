# 编译子系统使用 Kconfig 文件导出的变量

我们知道 Kconfig 系统会生成 .config 文件，在编译子系统里面，除了使用我们之前提到的

obj-y 

CFLAGS 

CFLAGS_EXTRA

CFLAG_obj

ASFLAGS

ASFLAGS_EXTRA

ASFLAG_obj

导出的各种全局变量

以外，我们还可以使用 .config  中的变量

本质和 obj-y 没有任何区别

使用 obj-${CONFIG_XXX} += yyy.o

或者使用 obj-${CONFIG_YYY} += aaa/

## 使用案例

打开 arch/Makefile 文件，如下内容

```makefile
obj-${CONFIG_ARCH_RISCV} += riscv/
obj-${CONFIG_ARCH_LOONGARCH} += loongarch/
```

我们会根据我们不同的配置来选择不同的目录参与到编译子系统