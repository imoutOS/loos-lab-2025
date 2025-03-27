# Kbuild System 用户使用编译指南 --- 用户初体验

## 前言

保证 X86 Host 需要具备以下的工具

- make
- gnu toolchain

我们的 Kbuild_System 在准备环境变量上和 Linux 一致，需要我们提前准备环境变量 ARCH 和 CROSS_COMPILE

- 临时准备环境变量 、此处以 riscv 为示例，若为 loongarch ，自行配置 ARCH=loongarch 以及 CROSS_COMPILE

  ```shell
  export ARCH=riscv
  export CROSS_COMPILE=riscv64-linux-gnu
  ```

- 持久性的环境变量，就是把上面的过程写到 ~/.bashrc 文件里面， 使用 vim（或其他编辑器） 打开 ~/.bashrc ，并将下面的内容添加到文件的最后位置，此处示例是 riscv ，若为其他架构，需要自己配置

  ```shell
  export ARCH=riscv
  export CROSS_COMPILE=riscv64-linux-gnu
  ```

- 检测自己是否配置好环境变量可以使用如下方法

  ```shell
  echo ${ARCH}
  echo ${CROSS_COMPILE}
  ```

  若两者均有输出，即为配置完成，比如我的结果是

  ```shell
  riscv
  riscv64-linux-gnu-
  ```

  

## 正文

### 使用 make help 查看支持的目标

- clone 项目到本地 (注：此处的 url 根据情况修改)

  ```shell
  git clone https://github.com/imoutOS/imoutOS.git
  ```

- 进入**项目根目录** 

  ```shell
  cd imoutOS
  ```

- 使用 make help 检查 Kbuild System 支持的目标

  ```shell
  make help
  ```

  下面是结果（结果根据当前 Kbuild System 支持的目标稍有不同，请注意甄别，此处仅仅做示例）

  ```shell
  Cleaning targets:
    clean           - Remove most generated files but keep the config and
                      enough build support to
    distclean       - mrproper + remove editor backup and patch files
  
  Other generic targets:
    compile         - Build the bare kernel
  
    menuconfig      - Configure Kernel config
  ```

### 使用 make 尝试编译

- 使用 make 执行默认的目标，默认目标为构建 kernel

  ```shell
  make
  ```

- 第一次执行 make 的时候会出现如下结果

  ```shell
  ***
  *** Configuration file "/home/jer6y/2025_kernel_comp/.config" not found!
  ***
  *** Please run some configurator (e.g. "make menuconfig")
  ***
  make: *** [Makefile:153：/home/jer6y/2025_kernel_comp/.config] 错误 1
  ```

  因为我们需要事先**自行配置内核**，大白话就是要事先决定好选择的 ARCH、选择的 BOARD、以及内核支持的特性等等。（配置使得内核裁剪起来非常简单方便），生成的结果会产出到 **项目根目录下的 .config 文件里面**

- 使用 make menuconfig 进行配置，自行选择配置选项

  ```shell
  make menuconfig
  ```

- 使用 make 进行编译

  ```shell
  make
  ```

### 使用 make menuconfig 进行配置

- 使用 make menuconfig 进行配置

  ```shell
  make menuconfig
  ```

  出现图形化界面后，根据自己的需求，进行对内核/板子的配置，完成后退出保存可以看到如下结果

  ```shell
  *** End of the configuration.
  *** Execute 'make' to start the build or try 'make help'.
  ```

  检查 **项目根目录** 下的 .config 文件，可以看到如下结果（此处仅做参考）

  ```shell
  #
  # Automatically generated file; DO NOT EDIT.
  # Main menu
  #
  
  #
  # arch specific config
  #
  CONFIG_ARCH_RISCV=y
  CONFIG_ARCH_TEST128273198=y
  CONFIG_BOARD_RV64QEMU_VIRT=y
  # CONFIG_BOARD_VISIONFIVE2 is not set
  CONFIG_DTS_PATH="/home/jer6y/2025_kernel_comp/arch/riscv/board/qemu_riscv64_virt/device.dts"
  # end of arch specific config
  ```

  #### **Note For Developer : config 文件不仅仅是用来参与 kbuild system 配置的，在你进行开发的时候也可以使用 config 里面的配置！！！！ 具体文件在 path2project/include/generated/autoconf.h , 比如我的这个文件如下**

  ```c
  #define CONFIG_ARCH_RISCV 1
  #define CONFIG_BOARD_RV64QEMU_VIRT 1
  #define CONFIG_DTS_PATH "/home/jer6y/2025_kernel_comp/arch/riscv/board/qemu_riscv64_virt/device.dts"
  #define CONFIG_ARCH_TEST128273198 1
  ```

### 使用 make clean 清除输出的结果

- 使用 make clean 清除输出结果

  ```shell
  make clean
  ```

  我的结果如下

  ```shell
  CLEAN		ELFS
  CLEAN		/home/jer6y/2025_kernel_comp/core
  CLEAN		/home/jer6y/2025_kernel_comp/driver
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev/linux
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/src
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4
  CLEAN		/home/jer6y/2025_kernel_comp/fs
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/struct
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/kstruct
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc
  CLEAN		/home/jer6y/2025_kernel_comp/mm
  CLEAN		/home/jer6y/2025_kernel_comp/arch/riscv
  CLEAN		/home/jer6y/2025_kernel_comp/arch
  ```

  clean 目标并不会清除产生的 .config ， autoconf.h 等文件，如果需要清理这些文件，需要使用 make distclean

### 使用 make distclean 清除干净输出的结果

- 使用 make distclean 清除输出结果

  ```shell
  make distclean
  ```

  我的结果如下

  ```shell
  CLEAN		ELFS
  CLEAN		/home/jer6y/2025_kernel_comp/core
  CLEAN		/home/jer6y/2025_kernel_comp/driver
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev/linux
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/src
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4
  CLEAN		/home/jer6y/2025_kernel_comp/fs
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/struct
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/kstruct
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc
  CLEAN		/home/jer6y/2025_kernel_comp/mm
  CLEAN		/home/jer6y/2025_kernel_comp/arch/riscv
  CLEAN		/home/jer6y/2025_kernel_comp/arch
  DISTCLEAN	CONFIG_FILES
  DISTCLEAN	GENERATED_DIR
  ```

### 使用 V=1 控制输出的日志效果

- 在上面所有的目标执行过程中，可以看到输出日志展示如下

  ```shell
  DISTCLEAN 	...
  CLEAN 		...
  CC			...
  AR			...
  LD			...
  ```

- 我们可以使用 make + 目标 V=1 展示不同的输出效果，例如

  ```
  make clean V=1
  make distclean V=1
  make V=1
  ```

  下面给一个具体的输出日志，执行 make clean V=1 时候的效果

  ```shell
  make -C /home/jer6y/2025_kernel_comp -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean clean
  make[1]: 进入目录“/home/jer6y/2025_kernel_comp”
  make -C /home/jer6y/2025_kernel_comp -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.elf; make -C /home/jer6y/2025_kernel_comp -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs;
  make[2]: 进入目录“/home/jer6y/2025_kernel_comp”
  rm -rf /home/jer6y/2025_kernel_comp/kernel
  CLEAN		ELFS
  make[2]: 离开目录“/home/jer6y/2025_kernel_comp”
  make[2]: 进入目录“/home/jer6y/2025_kernel_comp”
  make -C core -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/core”
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/core”
  CLEAN		/home/jer6y/2025_kernel_comp/core
  make -C driver -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/driver”
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/driver”
  CLEAN		/home/jer6y/2025_kernel_comp/driver
  make -C fs -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/fs”
  make -C lwext4 -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[4]: 进入目录“/home/jer6y/2025_kernel_comp/fs/lwext4”
  make -C blockdev -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[5]: 进入目录“/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev”
  make -C linux -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[6]: 进入目录“/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev/linux”
  rm -rf *.o
  rm -rf .*.d
  make[6]: 离开目录“/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev/linux”
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev/linux
  rm -rf *.o
  rm -rf .*.d
  make[5]: 离开目录“/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev”
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/blockdev
  make -C src -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[5]: 进入目录“/home/jer6y/2025_kernel_comp/fs/lwext4/src”
  rm -rf *.o
  rm -rf .*.d
  make[5]: 离开目录“/home/jer6y/2025_kernel_comp/fs/lwext4/src”
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4/src
  rm -rf *.o
  rm -rf .*.d
  make[4]: 离开目录“/home/jer6y/2025_kernel_comp/fs/lwext4”
  CLEAN		/home/jer6y/2025_kernel_comp/fs/lwext4
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/fs”
  CLEAN		/home/jer6y/2025_kernel_comp/fs
  make -C lolibc -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/lolibc”
  make -C struct -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[4]: 进入目录“/home/jer6y/2025_kernel_comp/lolibc/struct”
  rm -rf *.o
  rm -rf .*.d
  make[4]: 离开目录“/home/jer6y/2025_kernel_comp/lolibc/struct”
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/struct
  make -C kstruct -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[4]: 进入目录“/home/jer6y/2025_kernel_comp/lolibc/kstruct”
  rm -rf *.o
  rm -rf .*.d
  make[4]: 离开目录“/home/jer6y/2025_kernel_comp/lolibc/kstruct”
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc/kstruct
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/lolibc”
  CLEAN		/home/jer6y/2025_kernel_comp/lolibc
  make -C mm -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/mm”
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/mm”
  CLEAN		/home/jer6y/2025_kernel_comp/mm
  make -C arch -f /home/jer6y/2025_kernel_comp/scripts/Makefile.clean.objs
  make[3]: 进入目录“/home/jer6y/2025_kernel_comp/arch”
  rm -rf *.o
  rm -rf .*.d
  make[3]: 离开目录“/home/jer6y/2025_kernel_comp/arch”
  CLEAN		/home/jer6y/2025_kernel_comp/arch
  rm -rf *.o
  rm -rf .*.d
  make[2]: 离开目录“/home/jer6y/2025_kernel_comp”
  make[1]: 离开目录“/home/jer6y/2025_kernel_comp”
  ```
