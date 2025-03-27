# topMakefile 全局变量手册

[TOC]

## src_tree

- 功能 : 项目所在目录的**绝对路径**
- 说明 : 无



## CC

- 功能 :  gcc 工具
- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## CXX

- 功能 : g++ 工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## AR

- 功能 : ar 归档工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## LD

- 功能 : ld 链接工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## OBJDUMP

- 功能 : objdump 工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## OBJCOPY

- 功能 : objcopy 工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## NM

- 功能 : nm 工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## AS

- 功能 : as 工具

- 说明 : 根据 CROSS_COMPILE 会有不同前缀



## ECHO

- 功能 : echo 命令
- 说明 : 无



## RM

- 功能 : rm -rf 命令
- 说明 : 既可以删 目录，也可以删 文件



## MKDIR

- 功能: mkdir -p 命令
- 说明 : 可以直接创建类似 a/b/c 的级联目录



## DTC

- 功能: device tree compiler 工具
- 说明 : 这个工具使用的是源码的方式存在于项目，需要编译生成该工具，编译规则是 hosttool_compile 



## BASH

- 功能:  bash 脚本命令
- 说明 : 无



## MAKE

- 功能 : make 命令本身
- 说明 : 无



## CFLAGS

- 功能 :  全局的 GCC FLAG 
- 说明 :  基本只用在 编译子 Makefile 中



## ASFLAGS

- 功能 :  全局的 ASSEMBLY FLAG 
- 说明 :  基本只用在 编译子 Makefile 中



## LDFLAGS

- 功能 :  全局的 LD FLAG 
- 说明 :  基本只用在 编译子 Makefile 中



## Q

- 功能  :  控制输出格式， 一般在执行命令前面加上 $Q ，这样就可以通过 V=1, V=0 来控制输出格式 
- 说明  :  无



## TARGET

- 功能  :  编译子 Makefile 系统生成的 kernel elf 的绝对路径
- 说明  :  绝对路径



## KCONFIG_FILE

- 功能  :  kconfig 系统的 .config 文件的**绝对路径**
- 说明  : 无



## GENERATED_DIR

- 功能  ： kconfig 自动生成的 autoconf.h 等文件所在的目录的绝对路径
- 说明  :   只用于 distclean 子 Makefile 来清除对应的文件



## CONFIG_FILES

- 功能  ： kconfig 生产的 .config , .config.old 文件所在的绝对路径
- 说明  :   只用于 distclean 子 Makefile 来清除对应的文件



## KCONFIG_AUTOCONF

- 功能  : kconfig 生成的 auto.conf (用于Makefile 中解析 config 的选项内容) 的绝对路径
- 说明  :   应用于子 Makefile 系统 include 该文件来使用 config的内容



## LOTOOLCHAIN_DIR

- 功能 : loongarch 工具链的绝对路径
- 说明 : 提供给 loongarch download 的脚本来使用下载，同时不会被 distclean 和  clean 目标清除



## LOQEMU_DIR

- 功能 :  loongarch 工具链里面的 qemu 所在目录的绝对路径
- 说明 : 提供给 runqemu 目标来知道 qemu 所在路径



## LOOUTPUT_DIR

- 功能 ： loongarch 产物输出的目录的绝对路径，用于制造 rootfs 的image 等作用
- 说明 :  会被 clean 目标清除



# 导出规范

## 路径类

- 所有路径类的全局变量导出必须以绝对路径的形式
  - 比如 TARGET :  编译子 Makefile 系统生成的 kernel elf 的**绝对路径** 、 src_tree : 项目所在目录的**绝对路径**