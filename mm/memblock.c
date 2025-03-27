#include "memlayout.h"
#include "klib.h"
#include "mm.h"

extern char kernel_end[];

void freerange(void *pa_start, void *pa_end);

struct run {
    struct run *next;
};

struct {
    struct run *freelist;
} kmem;


#define KERNBASE   0x9000000090000000
#define KMEMSIZE   128*1024*1024
#define PHYSTOP    (KERNBASE + KMEMSIZE)
void kinit_memblock()
{
    freerange(kernel_end, (void*)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
    char *p;
    p = (char*)PGROUNDUP((uint64_t)pa_start);
    for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    {
        kfree_page_memblock(p);
    }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kmalloc(PGSIZE).  (The exception is when
// initializing the allocator; see kinit above.)
void kfree_page_memblock(void *pa)
{
    struct run *r;

    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    r->next = kmem.freelist;
    kmem.freelist = r;
}

// Allocate one 16K-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
// Normally this will directly panic.
void * kalloc_page_memblock(void)
{
    struct run *r;

    r = kmem.freelist;
    if ( !r)
        panic("kalloc");
    if(r) kmem.freelist = r->next;

    if(r) memset((char*)r, 5, PG_SIZE); // fill with junk

    // printf("kalloc %p\n", r);
    return (void*)r;
}
