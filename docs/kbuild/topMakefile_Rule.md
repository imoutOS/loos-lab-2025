# topMakefile 规则手册

[TOC]

## ${TARGET}

- 功能 :   编译出 kernel 的elf 文件的规则
- 说明 :  非 PHONY 规则



## compile

- 功能  :  距离编译出 kernel elf 文件少 一个 链接操作的规则
- 说明  :  PHONY 规则



## clean

- 功能  :  清除编译子系统产出的规则
- 说明  :  PHONY 规则



## distclean

- 功能  :  清除所有子系统产出的规则
- 说明  :  PHONY 规则



## kconfig_sync

- 功能  :  检测/同步 kconfig 的文件的规则 （.config ，autoconf.h 等），如果添加的某个规则需要依赖 .config 文件或者其产生的文件，在依赖的第一个加上这个依赖
- 说明  :  PHONY 规则



## help

- 功能  :  输出用户可使用的规则的帮助界面
- 说明  :  PHONY 规则



## hosttool_compile

- 功能 : 编译所有的需要的工具（源码在项目内的工具 如 dtc）
- 说明 : PHONY 规则



## lodownload

- 功能 : 下载 loongarch 需要的工具链
- 说明 :  下载失败会清除下载的内容，每次不会进行重复下载



## lorunqemu

- 功能 :  使用 qemu 运行生成的 loongarch 内核
- 说明 :  和 lodownload 只能在 loongarch 里面使用
