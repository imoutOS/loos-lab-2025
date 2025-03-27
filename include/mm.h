#ifndef MM_H
#define MM_H

#include "proc.h"
#include "memlayout.h"
#include "kstruct/list.h"

// memblock.c
void            kinit_memblock();
void *          kalloc_page_memblock(void);
void            kfree_page_memblock(void *pa);

// mm.c
void*           kernel_paddr(void*);
void*           kernel_vaddr(void*);
void            clean_dmw(void);
void            dmw_init(void);
void            tlb_init(void);
void            pmm_init(void);
void            dump_dmw();
void*           kmalloc(int);
void            kfree(void*);

// vm.c
void            vm_init(void);
pte_t *         va_to_pte_pointer(uint64_t * pagetable, uint64_t va, int alloc);
int             mappages(struct task_struct *, uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t        va_to_pa(pagetable_t pagetable, uint64_t va);
pagetable_t     pgtbl_create();
// uint64_t        __alloc_vm(pagetable_t pagetable, uint64_t start, uint64_t end, uint64_t perm){
uint64_t        alloc_vm(struct task_struct * proc, uint64_t start, uint64_t end, uint64_t perm);
int             copy_to_va(pagetable_t pagetable, uint64_t dstva, char *src, uint64_t len);
int             copy_from_va(pagetable_t pagetable, char *dst, uint64_t srcva, uint64_t len);
int             copy_from_va_str(pagetable_t pagetable, char *dst, uint64_t srcva, uint64_t max);
void            dump_mapping(struct task_struct*);
void pgtbl_free(struct task_struct *p) ;
void copy_pagetable(struct task_struct *p, struct task_struct *np) ;
void free_pagetable(pagetable_t pgtbl, int level);


// MAT, Memory Access Type

#define MAT_SUC 0
#define MAT_CC  1
#define MAT_WUC 2

static inline uint64_t __inverse_alloc(uint64_t alloc){
    return alloc == 1 ? 0 : 1;
}

#endif /* !MM_H */
