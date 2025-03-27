# Design Art of Kbuild System

### 核心要义

topMakefile 提供全局变量营造 **Kbuild System Makefile 的运行时**，子系统 或者称为 **子Makefile** 在运行时的基础上构建子系统，执行真正的规则

Kbuild System 是一个接口层，往下对接其他子系统的 Makefile , 往上提供用户接口

### Kbuild System Makefile 运行时

#### 用户的统一入口假象

- 用户接口  :  make + 规则 即为一切

#### 子Makefile 运行时

- topMakefile提供了所有的全局变量
- topMakefile提供了所有子系统的入口(执行子Makefile所在的目录)   --->  项目根目录
- 其他 子Makefile 系统提供的 变量/函数/规则
  - scripts/Makefile.include 提供了所有可用的函数
  - Kconfig Makefile 子系统提供可用的Kconfig 变量



### Design art of Kbuild Compile System

### Design art of Kbuild Clean System

### Design art of Kconfig System