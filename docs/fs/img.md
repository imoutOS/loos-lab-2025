# Build Image for LoOS

## ext4

### lwext4

LoOS 使用了 lwext4 库作为 ext4 文件系统的具体实现。因此，在创建 ext4 镜像时，
需要注意不要使用 lwext4 库不支持的特性。

以下是一份如何使用 mkfs.ext4 生成 lwext4 兼容镜像的命令：

> mkfs.ext4 -O^ext_attr,^resize_inode,^orphan_file,^64bit,^flex_bg,^metadata_csum_seed,^huge_file,^dir_nlink,^extra_isize,^metadata_csum sdcard-bb-ext4-no-features.img

misane 创建了一份用于测试 busybox 的 FAT32 格式的镜像 sdcard-bb.img 。可以从
tiger1218.com/files/sdcard-bb.img 获取到。为了在 ext4 上测试 busybox ，我们可
以进行如下操作：

```bash
qemu-img create sdcard-bb-ext4.img 2G -f raw
mkfs.ext4 sdcard-bb-ext4.img # 该步需要替换为上述的命令以兼容 lwext4
sudo mount sdcard-bb.img /mnt
sudo mount sdcard-bb-ext4.img /media
sudo cp -r /mnt/* /media
```

可以通过 `dumpe2fs sdcard-bb-ext4.img | less` 的 features 字段检查是否去除了
lwext4 不兼容的特性。

### VFS
