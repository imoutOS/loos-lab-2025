# 全国大学生计算机系统能力大赛-操作系统设计赛(全国)-OS内核实现赛道

## 评测

### RISC-V

本次大赛的初赛阶段评测使用QEMU虚拟环境，提交的项目根目录中必须包含一个Makefile文件，评测时会自动在您的项目中执行make all命令，您应该在Makefile中的all目标对操作系统进行编译，并生成ELF格式的sbi-qemu和kernel-qemu两个文件，即与xv6-k210运行qemu时的方式一致。

如果你的系统使用默认SBI，则不需要生成sbi-qemu文件，运行QEMU时会自动设置-bios参数为default。

同时QEMU启动时还会使用-drive file参数挂载SD卡镜像，SD卡镜像为FAT32文件系统，没有分区表。在SD卡镜像的根目录里包含若干个预先编译好的ELF可执行文件（以下简称测试点），您的操作系统在启动后需要主动扫描SD卡，并依次运行其中每一个测试点，将其运行结果输出到串口上，评测系统会根据您操作系统的串口输出内容进行评分。您可以根据操作系统的完成度自由选择跳过其中若干个测试点，未被运行的测试点将不计分。测试点的执行顺序与评分无关，多个测试点只能串行运行，不可同时运行多个测试点。具体测试点的数量、内容以及编译方式将在赛题公布时同步发布。

当您的操作系统执行完所有测试点后，应该主动调用关机命令，评测机会在检测到QEMU进程退出后进行打分。

运行Riscv QEMU的完整命令为：

#### 评测命令

qemu-system-riscv64 -machine virt -kernel kernel-qemu -m 128M -nographic -smp 2 -bios sbi-qemu -drive file=sdcard.img,if=none,format=raw,id=x0  -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 -device virtio-net-device,netdev=net -netdev user,id=net -initrd initrd.img

root@ed607ece8a46:/# qemu-system-riscv64 --version
QEMU emulator version 7.0.0
Copyright (c) 2003-2022 Fabrice Bellard and the QEMU Project developers

在运行QEMU的命令中，-initrd initrd.img为可选项。如果你的Makefile生成了initrd.img，则会在运行命令中添加此参数，否则运行命令中不包含-intird initrd.img参数。

### LoongArch 赛道

#### 评测套件分析

https://github.com/LoongsonLab/oscomp-testsuits-loongarch

#### 评测命令

>     使用LoongArch进行评测时，您编译出的操作系统内核应该为二进制格式的kernel.bin，而不是Riscv平台中的ELF格式。并且需要将您内核的起始地址放在0x9000000008000000。Loong平台的操作系统内核统一使用Uboot引导，不支持自行生成SBI文件。评测程序的挂载与评分方式与Riscv平台相同。

```bash
qemu-system-loongarch64 -M ls2k -serial stdio -serial vc -drive if=pflash,file=/tmp/qemu/2k1000/u-boot-with-spl.bin -m 1024 -device usb-kbd,bus=usb-bus.0 -device usb-tablet,bus=usb-bus.0 \
    -device usb-storage,drive=udisk -drive if=none,id=udisk,file=/tmp/disk -net nic -net user,net=10.0.2.0/24,tftp=/srv/tftp -vnc :0 -D /tmp/qemu.log \
    -s -hda /tmp/qemu/2k1000/2kfs.img -hdb file=sdcard.img

qemu-system-loongarch64 --version
QEMU emulator version 3.1.0 (v3.1.0-334-g3192af67aa-lvz1-dirty)
Copyright (c) 2003-2018 Fabrice Bellard and the QEMU Project developers
```

您的项目中如果需要引用第三方工具/库，请以源代码的方式提交并与内核一同编译，不应该直接提交二进制文件。


评测系统在Clone你的项目时，会过滤掉目录中所有的隐藏文件和目录，如果你的程序在构建时一定依赖隐藏目录，如.cargo，则提交的项目中可以将该目录修改为其他名字，在Makefile中再将其修改为.cargo。


本题目评测时间为5分钟，即编译时间+运行时间一共不能超过5分钟，否则会提示“评测时间过长”。出现此错误可能的原因有：1. 未将所需的第三方包放在项目中，导致编译时在线下载依赖时间过长。2. 运行结束后没有及时关闭QEMU。3. 提交的代码中存在BUG导致运行速度过慢。

### 在线评测环境

点击“在线IDE”按钮即可打开在线实验环境。开发板的串口被连接到了/dev/ttyUSB1，你可以使用minicom -D /dev/ttyUSB1 -b 115200访问开发板。

通过/cg/control程序可以控制开发板的电源。在终端使用 /cg/control on命令可以启动开发板电源，使用/cg/control off 命令可以关闭电源。

本环境已经内置了tftp服务器，可以通过tftp将文件传到开发板上。tftp 的服务目录为/srv/tftp/xxx ，其中xxx为开发板的编号，在vscode中tftp目录下应该有且仅有一个子目录，将需要传输的文件放到子目录中即可。

通过tftp下载文件时需要附带子目录名，例如，你将os.bin文件放在了/srv/tftp/s1/os.bin，启动开发板后在uboot中可以使用dhcp获得IP地址，随后使用 bootp 0x80200000 s1/os.bin下载此文件。


如果在minicom中出现无法输入，输出内容不全等问题，可以检查硬件流控是否被关闭。检查方法：

按Ctrl+a o打开设置界面，选择Serial Port setup，然后按F将Hardware Flow Control设置为No即可。

### C/C++ 开发环境

https://github.com/LoongsonLab/oscomp-toolchains-for-oskernel

### Rust 开发环境

nightly-2024-02-03

安装RISC-V 平台:

rustup target add riscv64imac-unknown-none-elf --toolchain nightly-2024-02-03 rustup target add riscv64gc-unknown-none-elf --toolchain nightly-2024-02-03 rustup component add llvm-tools-preview --toolchain nightly-2024-02-03

已安装Rust 社区提供的 cargo-binutils 项目，帮助调用 Rust 内置的 LLVM binutils：

rustup install nightly-2024-02-03 rustup default nightly-2024-02-03 rustup component add llvm-tools-preview rustup target add riscv64imac-unknown-none-elf rustup target add riscv64gc-unknown-none-elf

内置的RUST依赖库为：

rustsbi="0.3.2"
futures-lite="1.12.0"
crossbeam-queue="0.3.8"
heapless="0.7.16"
async-task="4.4.0"
bitflags="2.0.2"
log = "0.4"
num_enum = { version = "0.5.11", default-features = false }

### 评测镜像下载

docker pull docker.educg.net/cg/os-contest:2024p6

Dockerfile : https://gitlab.educg.net/wangmingjian/os-contest-2024-image
