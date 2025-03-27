#include "disk.h"
#include "klib.h"
#include "bits.h"
#include "vm.h"
#include "memlayout.h"
#include "fs.h"
#include "ext2.h"

char ramdisk[] = {0};


int ramdisk_read(void *buf, int offset, int len) {
    memcpy(buf, ramdisk + offset, len);
    return len;
}

int ramdisk_write(void *buf, int offset, int len) {
    memcpy(ramdisk + offset, buf, len);
    return len;
}

void disk_init() {
    uint8_t buf[8192];
    HBA_MEM *abar = (void*)SATA_ADDR;
    int port_num = ahic_probe_port(abar);  // port 1, disk hdb
    ahci_read(abar, port_num, 0, 8, buf);
    if ( ext2_check(buf) ) {
        ext2_init_disk(abar, port_num);
    } else {
        panic("unknown fs");
    }
}
