# LoOS

## 概览

This repo store the source code of LoOS, a LoongArch based operating system.

## 构建系统/开发环境搭建
[开发运行环境](docs/User_use/User_use.md)

### GCC/Binutils 工具链

### GDB 工具链

### U-Boot

### QEMU 模拟器

### Makefile 与代码层次结构
 
## 文档

### LoongArch 体系结构

mainly the notes learning LoongArch ISA.

[龙芯体系结构概览](docs/loongarch/index.md)

[龙芯体系结构 ABI](docs/loongarch/abi.md)

[龙芯体系结构整数指令概括](docs/loongarch/int.md)

[龙芯体系结构内存管理](docs/loongarch/mm.md)

[龙芯体系结构特权体系结构](docs/loongarch/priv.md)

### 评测

[评测概览](docs/os/index.md)

[LoongArch 相关工具链与评测](docs/os/loongarch.md)

### 内存管理

[内存管理概览](docs/mm/index.md)

[内存管理常用宏](docs/mm/index.md)

### lolibc

### 驱动相关

### 文件系统

## 软件工程

### Branch 规范

`master` 分支用于存储稳定的 LoOS 版本，其内容需要确保能够编译。

`devel` 分支用于存储最新的 LoOS 开发版本。

`judge-deploy` 分支用于存储评测时使用的分支。理论上其只能是对 `master` 分支的同
### Branch 规范

`master` 分支用于存储稳定的 LoOS 版本，其内容需要确保能够编译。

`devel` 分支用于存储最新的 LoOS 开发版本。

`judge-deploy` 分支用于存储评测时使用的分支。理论上其只能是对 `master` 分支的同步。

其中，`master` 分支和 `devel` 分支是 "protected branch" ，对它们两个分支的修改必须经过
“ 新建针对某特性开发的 branch ” - “ 发起 merge request ” - “ maintainer 将其
merge 进入 devel/master ” 的过程。

### Commit 规范

[Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)
步。

其中，`master` 分支和 `devel` 分支是 "protected branch" ，对它们两个分支的修改必须经过
“ 新建针对某特性开发的 branch ” - “ 发起 merge request ” - “ mastertainer 将其
merge 进入 devel/master ” 的过程。

### Commit 规范

[Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/)
