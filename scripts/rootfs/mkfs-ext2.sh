#! /bin/sh

qemu-img create rootfs.img 256M -f raw

mkfs.ext2 \
    -O^ext_attr,^dir_index,^sparse_super,^filetype,^resize_inode,^large_file rootfs.img
