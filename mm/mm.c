#include "klib.h"
#include "bits.h"
#include "mm.h"
#include "memlayout.h"
#include "asm/tlb.h"

extern char kernel_end[];

static inline uint64_t get_dmw_data(uint32_t base, uint8_t mat, uint8_t plv) {
    uint64_t res = ((uint64_t)base << VALEN) | ((uint64_t)mat << 4) | (uint64_t)plv;
    return res;
}

void dmw_init() {
    w_csr_dmw0(get_dmw_data(0x9000, MAT_CC, DMW_PLV0 | DMW_PLV1 | DMW_PLV2 | DMW_PLV3));
    // printf("write dmw0: %x\n", get_dmw_data(0x9000, MAT_CC, 0xF));
    // printf("write dmw1: %x\n", get_dmw_data(0xA000, MAT_CC, DMW_PLV0 | DMW_PLV1 | DMW_PLV2 | DMW_PLV3));
    // printf("write dmw2: %x\n", get_dmw_data(0x8000, MAT_SUC, DMW_PLV0 | DMW_PLV1 | DMW_PLV2 | DMW_PLV3));
    w_csr_dmw1(get_dmw_data(0xA000, MAT_CC, DMW_PLV0 | DMW_PLV1 | DMW_PLV2 | DMW_PLV3));
    w_csr_dmw2(get_dmw_data(0x8000, MAT_SUC, DMW_PLV0 | DMW_PLV1 | DMW_PLV2 | DMW_PLV3));
    // dump_dmw();
}

void tlb_init() {
    w_csr_stlbps(0xE);
    invtlb_all();
    w_csr_asid(0x0U);//todo
}

void pmm_init()
{
    kinit_memblock();
}

void * kmalloc(int size){
    if ( size > PGSIZE ) panic("too big alloc request!");
    return kalloc_page_memblock();
}

void kfree(void * paddr){
    if ( (uint64_t)paddr % PGSIZE == 0 ) {
        kfree_page_memblock(paddr);
    } else {
        panic("unaligned kfree request");
    }
}
