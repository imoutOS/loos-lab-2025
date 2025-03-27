#! /bin/sh
#
# fhs.sh
# Copyright (C) 2024 Tiger1218 <tiger1218@foxmail.com>
#
# Distributed under terms of the GNU AGPLv3 license.
#

BUSYBOX=/home/tiger1218/repos/oscomp-testsuits-loongarch/busybox
SYSROOT=/home/tiger1218/repos/os-contest-2024-image/gcc-13.2.0-loongarch64-linux-gnu-nw/sysroot

mkdir -p /media/usr
# mkdir -p /media/usr/bin
# mkdir -p /media/usr/lib
# ln -sf /media/usr/bin /media/bin
# ln -sf /media/usr/lib /media/lib
mkdir -p /media/bin
mkdir -p /media/lib
mkdir -p /media/etc
mkdir -p /media/opt
mkdir -p /media/run
mkdir -p /media/var

cd $BUSYBOX; make install CONFIG_PREFIX=/media
cp -r $SYSROOT/etc /media/etc
cp $SYSROOT/sbin/* /media/bin
cp -r $SYSROOT/usr /media/usr
cp -r $SYSROOT/var /media/var
