// #include "vm.h"

#include "memlayout.h"
#include "klib.h"
#include "mm.h"
#include "proc.h"
#include "asm/mmu.h"
#include "struct/mlist.h"

// #define Log printf

extern char kernel_end[];

void tlbinit(void)
{
}

void __proc_add_mapping(struct task_struct *proc, uint64_t pa, uint64_t va, uint64_t size, uint64_t perm) {
    struct mapping *mapping = (void*)malloc(sizeof(struct mapping));
    // printf("map: %p, va %p, pa %p\n", mapping, va, pa);
    mapping->pa = pa;
    mapping->va = va;
    mapping->size = size;
    mapping->perm = perm;
    mlist_push(proc->mapping_list, mapping);
}

int __map_single_page(pagetable_t pagetable, uint64_t va, uint64_t pa, uint64_t perm){
    Log("map_single_page: %p to %p\n", pa, va);
    // uint64_t a, last;

    pagetable_t iter_pagetable = pagetable;
    Log("pagetable: %p, iter_pagetable: %p\n", iter_pagetable, pagetable);
    for(int level = 2; level >= 1 ; level -- ){
        uint64_t index = PX(level, va);
        Log("index of level %d is %x\n", level, index);
        uint64_t pte = iter_pagetable[index];
        if ( pte == 0 ){
            Log("no pte for current layer, allocing\n");
            void * new_pagetable = kmalloc(PGSIZE);
            memset(new_pagetable, 0, PG_SIZE);
            iter_pagetable[index] = (uint64_t) new_pagetable;
        }
        pte = iter_pagetable[index];
        Log("level %d pagetable(%p) [%d] is assign to %p\n", level, iter_pagetable, index, pte);
        iter_pagetable = (uint64_t *)pte;
    }
    // uint64_t t_pte = iter_pagetable[PX(0, va)];
    iter_pagetable[PX(0, va)] = PA2PTE(pa) | PTE_FLAGS(iter_pagetable[PX(0, va)]) | perm;
    Log("final level, index %d is assign to %x\n", PX(0, va), iter_pagetable[PX(0,va)]);
    Log("perm is %d\n", perm);
    return 0;
}

/*int mappages(struct proc * proc, uint64_t va, uint64_t size, uint64_t pa, uint64_t perm){
    __map_pages(proc->pgtbl, va, size, pa, perm);
    __proc_add_mapping(proc, pa, va, size, perm);
    return 0;
}*/

void vm_init(void)
{
    w_csr_pwcl((PTEWIDTH << 30)|(DIR2WIDTH << 25)|(DIR2BASE << 20)|(DIR1WIDTH << 15)|(DIR1BASE << 10)|(PTWIDTH << 5)|(PTBASE << 0));
    w_csr_pwch((DIR4WIDTH << 18)|(DIR3WIDTH << 6)|(DIR3BASE << 0));
}

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
pte_t * va_to_pte_pointer(pagetable_t pagetable, uint64_t va, int alloc)
{
    // Log("walk pagetable %p from va %p, alloc = %d\n", pagetable, va,alloc);
    if(va >= MAXVA)
      panic("walk");
    
    pagetable = (void*)PA2VA(pagetable);
    for(int level = 2; level > 0; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        // Log("&pte = %p pagetable = %p *pte = %p\n", pte, pagetable, *pte);
        if(*pte) {
            pagetable = (pagetable_t)(PTE2PA(*pte) );
            pagetable = (void*)PA2VA(pagetable);
        } else {
            if(!alloc || (pagetable = (pte_t*)kmalloc(PGSIZE)) == 0)
                return 0;
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    // Log("final pte = %p, *pte = %p\n", pagetable[PX(0,va)], &pagetable[PX(0,va)]);
    return &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64_t va_to_pa(pagetable_t pagetable, uint64_t va)
{
    uint64_t offset = va % PGSIZE;
    va = va - offset;
    pte_t *pte;
    uint64_t pa;

    if(va >= MAXVA)
        return 0;

    pte = va_to_pte_pointer(pagetable, va, 0);
    if(pte == 0) return 0;
    Log("va is %p\n", va);
    Log("*pte is %p\n", *pte);
    Log("*pte flags are %x\n", PTE_FLAGS((uint64_t)(*pte)));
    if((*pte & PTE_V) == 0){
        Log("PTE_V of %p is 0\n", pte);
        return 0;
    }
    pa = PTE2PA(*pte);
    return (uint64_t)pa + offset;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned. Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
int __map_pages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, uint64_t perm)
{
    Log("mappages: %p to %p, length = %d\n", pa, va, size);
    uint64_t va_start = PGROUNDDOWN(va);
    uint64_t va_end = PGROUNDDOWN(va + size - 1);
    uint64_t pa_map = PGROUNDDOWN(pa);
    Log("va_start = %p, va_end = %p, pa_map = %p\n", va_start, va_end, pa_map);
    for (uint64_t vac = va_start ; vac <= va_end ; vac += PGSIZE) {
        // Log("into the circle\n");
        __map_single_page(pagetable, vac, pa_map, perm);
        pa_map += PGSIZE;
    }
    return 0;
}

pagetable_t pgtbl_create(){
    pagetable_t pgtbl = (pagetable_t) kmalloc(PGSIZE);
    if ( pgtbl ){
        memset(pgtbl, 0, PGSIZE);
        return pgtbl;
    }
    return 0;
}

// FIXME: leak memory
void pgtbl_free(struct task_struct *p) {
    struct mapping *info = NULL;
    void *list = p->mapping_list;
    while ( !mlist_is_empty(list)) {
        info = mlist_pop(list);
        void *pa = (void*)PA2VA(info->pa);
        kfree(pa);
        free(info);
    }
    free_pagetable(p->pgtbl, 2);
}

// Allocate PTEs and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64_t alloc_vm(struct task_struct * proc, uint64_t start, uint64_t end, uint64_t perm){
    pagetable_t pagetable = proc->pgtbl;
    Log("alloc vm from pagetable %p , start %p end %p\n", pagetable, start, end);
    end = PGROUNDUP(end);
    for ( uint64_t caddr = start ; caddr < end ; caddr += PGSIZE ){
        void * new_mem = kmalloc(PGSIZE);
        if ( !new_mem ) panic("kalloc");
        memset(new_mem, 0, PGSIZE);
        if ( __map_single_page(pagetable, caddr, (uint64_t)new_mem, perm) != 0 ) panic("__map_single_page");
        __proc_add_mapping(proc, (uint64_t)new_mem, caddr, PGSIZE, perm);
    }
    return 0;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
int copy_to_va(pagetable_t pagetable, uint64_t dstva, char *src, uint64_t len)
{
    uint64_t n, va0, pa0;

    while(len > 0){
        va0 = PGROUNDDOWN(dstva);
        pa0 = va_to_pa(pagetable, va0);
        if(pa0 == 0) return -1;
        n = PGSIZE - (dstva - va0);
        if(n > len) n = len;
        Log("cur pa0 = %p, dstva = %p, va0 = %p\n", pa0, dstva, va0);
        Log("copyout to %p with src = %p with n = %d\n", (void *)PA2VA((pa0 + (dstva - va0))), src, n);
        memmove((void *)PA2VA((pa0 + (dstva - va0))), src, n);

        len -= n;
        src += n;
        dstva = va0 + PGSIZE;
    }
    return 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
int copy_from_va(pagetable_t pagetable, char *dst, uint64_t srcva, uint64_t len)
{
    uint64_t n, va0, pa0;

    while(len > 0){
        va0 = PGROUNDDOWN(srcva);
        pa0 = va_to_pa(pagetable, va0);
        if(pa0 == 0) return -1;
        n = PGSIZE - (srcva - va0);
        if(n > len)  n = len;
        Log("copyin %p\n", (void *)PA2VA((pa0 + (srcva - va0))));
        memmove(dst, (void *)PA2VA((pa0 + (srcva - va0))), n);
        len -= n;
        dst += n;
        srcva = va0 + PGSIZE;
    }
    return 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int copy_from_va_str(pagetable_t pagetable, char *dst, uint64_t srcva, uint64_t max)
{
    uint64_t n, va0, pa0;
    int got_null = 0;

    while(got_null == 0 && max > 0){
        va0 = PGROUNDDOWN(srcva);
        pa0 = va_to_pa(pagetable, va0);
        if(pa0 == 0) return -1;
        n = PGSIZE - (srcva - va0);
        if(n > max) n = max;

        char *p = (char *) PA2VA((pa0 + (srcva - va0)));
        while(n > 0){
            if(*p == '\0'){
                *dst = '\0';
                got_null = 1;
                break;
            } else {
                *dst = *p;
            }
            --n;
            --max;
            p++;
            dst++;
        }

        srcva = va0 + PGSIZE;
    }
    if(got_null){
        return 0;
    } else {
        return -1;
    }
}//todo

void dump_mapping(struct task_struct * proc){
    /*
    for ( int i = 0 ; i < 256 ; i ++){
        if ( !proc->mapping[i].used ) continue;
        printf("for mapping index %d: ", i);
        printf("va = %p, pa = %p, size = %p, perm = %p\n", proc->mapping[i].va,
                                                           proc->mapping[i].pa,
                                                           proc->mapping[i].size,
                                                           proc->mapping[i].perm);
    }
    */
}

void copy_pagetable(struct task_struct *p, struct task_struct *np) {
    // assume np->pgtbl and mapping_list exist
    void *old_list = p->mapping_list;
    p->mapping_list = mlist_init();

    struct mapping *info = NULL;
    while ( !mlist_is_empty(old_list)) {
        info = mlist_pop(old_list);

        void *new_mem = kmalloc(PGSIZE);
        memset(new_mem, 0, PG_SIZE);
        copy_from_va(p->pgtbl, new_mem, info->va, PG_SIZE);
        __map_pages(np->pgtbl, info->va, info->size, (uint64_t)new_mem, info->perm);

        void *pa = (void*)PA2VA(va_to_pa(np->pgtbl, info->va));
        __proc_add_mapping(p, info->pa, info->va, PG_SIZE, info->perm);
        __proc_add_mapping(np, (uint64_t)pa, info->va, PG_SIZE, info->perm);
    }
    // FIXME: THIS CAUSE BUG
    // free(old_list);
}

/* 
 * best practice of copy_pagetable is:
 * 1. first copy the whole proc, like memcpy(new_proc, old_proc, sizeof(proc);
 * 2. which means you copied the whole mapping array,
 * 3. then you can use the result of copy_pagetable(new_proc) for your final
 *              pagetable
 */

void free_pagetable(pagetable_t pgtbl, int level){
    if ( level == 0 ) {
        kfree(pgtbl);
        return ;
    }
    for ( int i = 0 ; i < (1 << PTWIDTH) ; i ++ ){
        pagetable_t it_pgtb = (pagetable_t)pgtbl[i];
        if ( it_pgtb ) free_pagetable(it_pgtb, level - 1);
    }
    kfree(pgtbl);
}

int check_va(uint64_t ptr){
    return 1;
}
