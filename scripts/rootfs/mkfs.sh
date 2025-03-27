#! /bin/sh

qemu-img create rootfs.img 256M -f raw

mkfs.ext4 \
    -O^ext_attr,^resize_inode,^orphan_file,^64bit,^flex_bg,^metadata_csum_seed,^huge_file,^dir_nlink,^extra_isize,^metadata_csum rootfs.img
