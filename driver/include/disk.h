#ifndef DISK_H
#define DISK_H
#include <klib.h>
#include <ahci.h>
#include <bits.h>
#define CONFIG_RAMDISK_SIZE (50 * 1024 * 1024)
extern char ramdisk[CONFIG_RAMDISK_SIZE];

int ramdisk_read(void *buf, int offset, int len);
int ramdisk_write(void *buf, int offset, int len);

#endif
