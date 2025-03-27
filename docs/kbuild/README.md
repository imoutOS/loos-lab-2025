# Kbuild System

## 概览

Kbuild **构建系统** 

**特点**

- **多目标**
-  **Mconf 配置**
- **高度可扩展**
- **并行构建支持**
- **高度可裁剪**

**约束限制**

- 只支持 Host 为 x86 



## 用户指南

#### Makefile 规则篇

- [用户使用编译指南](./User_use.md)
- [如何添加一个新的 Makefile 规则供给用户使用](./add_new_target.md)
- [topMakefile 全局变量手册](./topMakefile_Var.md)
- [topMakefile 规则手册](./topMakefile_Rule.md)

#### 编译子系统篇

- [如何添加一个新的目录/文件到编译系统内](./cmsystem_add.md)
- [如何添加新的 CFLAG、ASFLAG、和 LDFLAG](./cmsystem_flags.md)
- [如何使用 Kconfig 导出的变量](./cmsystem_usekconfig.md)



## 设计理念

- [Design Art of Kbuild System](./artof_kbuild.md)

