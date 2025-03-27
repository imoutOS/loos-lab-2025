#include "klib.h"
#include "ahci.h"
#include "bits.h"
#include "memlayout.h"
 
 // Search disk in implemented ports
int ahic_probe_port(HBA_MEM *abar) {
	int port_no = -1;
	int port_num = 0;
	uint32_t pi = abar->pi;

	for (int i=0; i < 32; i++) {
		if ((pi & 1) == 0)
            continue;
        
        HBA_PORT *port = (void*)&abar->ports[i];
        uint32_t ssts = port->ssts;

        uint8_t ipm = (ssts >> 8) & 0x0F;
        uint8_t det = ssts & 0x0F;
    
        if (det != HBA_PORT_DET_PRESENT  || ipm != HBA_PORT_IPM_ACTIVE)	{
            continue;
        }
    
        Log(
            "%s drive found at port %d\n",
            (port->sig == SATA_SIG_ATAPI) ? "SATAPI" :
            (port->sig == SATA_SIG_SEMB) ? "SEMB" :
            (port->sig == SATA_SIG_PM) ? "PM" : "SATA",
            i
        );

		port_num++;
		if (port_num == 2)
			port_no = i;
	}
    // port_no = 0;
	return port_no;
}

// Find a free command list slot
static int find_cmdslot(HBA_MEM *abar, volatile HBA_PORT *port) {
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	int cmdslots = get_bits_range(abar->cap, 8, 4); // bit 12-8
	for (int i=0; i < cmdslots; i++) {
		if ((slots & 1) == 0)
			return i;
		slots >>= 1;
	}
	Log("Cannot find free command list entry\n");
	return -1;
}

// From https://wiki.osdev.org/AHCI
// read "count" sectors from sector offset "starth:startl" to "buf" with LBA48 mode from HBA "port".
// Every PRDT entry contains 8K bytes data payload at most. 
int do_ahci_read(HBA_MEM *abar, int port_num, uint64_t start, uint32_t count, uint8_t *buf) {
	volatile HBA_PORT *port = &(abar->ports[port_num]);
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(abar, port);
	if (slot == -1) {
		Log("find slot failed\n");
		return -1;
	}
 
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)PHYS_TO_UNCACHED(port->clb);
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)PHYS_TO_UNCACHED(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
	// 8K bytes (16 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)(uint64_t)buf;
		cmdtbl->prdt_entry[i].dbau = (uint32_t)(((uint64_t)buf)>>32);
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 0x1000;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t)(uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EXT;

	uint32_t startl = start,
		starth = start >> 32;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin++;
	}
	if (spin == 1000000) {
		Log("Port is hung\n");
		return -1;
	}
	port->ci = 1<<slot;	// Issue command
	// Wait for completion
	//Log("ahci disk wait for completion\n");
	while (1) {
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES) {
			// Task file error
			Log("Read disk error\n");
			return -1;
		}
	}
	// Check again
	if (port->is & HBA_PxIS_TFES) {
		Log("Read disk error\n");
		return -1;
	}
	return 0;
}

uint8_t ahci_buf[8192];
int ahci_read(HBA_MEM *abar, int port_num, uint64_t start, uint32_t count, uint8_t *buf) {
	int ret = do_ahci_read(abar, port_num, start, count, ahci_buf);
	memcpy(buf, ahci_buf, count*512);
	return ret;
}

int do_ahci_write(HBA_MEM *abar, int port_num, uint64_t start, uint32_t count, uint8_t *buf) {
	volatile HBA_PORT *port = &(abar->ports[port_num]);
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(abar, port);
	if (slot == -1) {
		Log("find slot failed\n");
		return -1;
	}
 
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)PHYS_TO_UNCACHED(port->clb);
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
 
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)PHYS_TO_UNCACHED(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
	// 8K bytes (16 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)(uint64_t)buf;
		cmdtbl->prdt_entry[i].dbau = (uint32_t)(((uint64_t)buf)>>32);
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 0x1000;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t)(uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EXT;

	uint32_t startl = start,
		starth = start >> 32;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin++;
	}
	if (spin == 1000000) {
		Log("Port is hung\n");
		return -1;
	}
	port->ci = 1<<slot;	// Issue command
	// Wait for completion
	//Log("ahci disk wait for completion\n");
	while (1) {
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES) {
			// Task file error
			Log("Read disk error\n");
			return -1;
		}
	}
	// Check again
	if (port->is & HBA_PxIS_TFES) {
		Log("Read disk error\n");
		return -1;
	}
	return 0;
}

// not test
int ahci_write(HBA_MEM *abar, int port_num, uint64_t start, uint32_t count, uint8_t *buf) {
	memcpy(ahci_buf, buf, count*512);
	int ret = do_ahci_write(abar, port_num, start, count, ahci_buf);
	return ret;
}
