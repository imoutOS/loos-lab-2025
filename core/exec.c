#include "exec.h"
#include "klib.h"
#include "memlayout.h"
#include "proc.h"
#include "fs.h"
#include "asm/mmu.h"
#include "mm.h"
#include "bits.h"
#include <elf.h>

#define Elf_Ehdr Elf64_Ehdr
#define ELFCLASS ELFCLASS64
#define Elf_Phdr Elf64_Phdr


uint8_t elf_buf[20*1024*1024];
const uint8_t elf_magic[] = {0x7f, 'E','L','F'};
extern char _kernel_start[], _kernel_end[];

static void print_segment_info(Elf_Phdr *segment) {
    Log("type %p\toffset %p\tvaddr %p\tpaddr %p\tfilesz %p\tmemsz %p\tflags %p\talign %p\n",
            segment->p_type, segment->p_offset, segment->p_vaddr, segment->p_paddr,
            segment->p_filesz, segment->p_memsz, segment->p_flags, segment->p_align);
}
uintptr_t elf_loader(struct task_struct *p, char *path) {
    int myfd = openat(0, path, 0);
    if (myfd == -1) return -1;
    read(myfd, elf_buf, sizeof(elf_buf));
    close(myfd);
    // memcpy(elf_buf, demo, sizeof(demo));

    const uint8_t *elf_base = elf_buf;
    assert(elf_base);

    /* parse header */
    Elf_Ehdr *header = (Elf_Ehdr*)elf_base;
    assert(memcmp(header->e_ident, elf_magic, sizeof(elf_magic)) == 0);
    assert(header->e_machine == EM_LOONGARCH);
    assert(header->e_ident[EI_CLASS] == ELFCLASS);
    assert(header->e_type == ET_EXEC || header->e_type == ET_DYN);
    if (memcmp(header->e_ident, elf_magic, sizeof(elf_magic)))
        return -1;

    /* find segment */
    Elf_Phdr *segments = (Elf_Phdr*)(elf_base + header->e_phoff);
    size_t phnum = header->e_phnum, ldnum = 0;
    Log("find %p segment(s) from elf\n", phnum);
    assert(phnum);

    /* load segment to memory */
    for (int i=0; i < phnum; i++) {
        Elf_Phdr *segment = &segments[i];
        // ignore p_type etc.
        uint8_t *dst = (uint8_t*)segment->p_vaddr;
        const uint8_t *src = elf_base + segment->p_offset;
        size_t len = segment->p_filesz;
        
        print_segment_info(segment);
        if (segment->p_type == PT_LOAD) {
            // assert(dst);
            assert(segment->p_filesz <= segment->p_memsz);
            assert(len);

            uint64_t pg_size = (PG_SIZE > segment->p_align) ? PG_SIZE : segment->p_align;
            void *vaddr = (void*)dst;
            uintptr_t start = ALIGN((uintptr_t)dst, pg_size);
            uintptr_t end = ALIGN((uintptr_t)dst + segment->p_memsz -1, pg_size) + pg_size;
            Log("vaddr %p, start %p, end %p\n", vaddr, start, end);

            if ((segment->p_flags & PF_R ) && (segment->p_flags & PF_W)) {
                p->bss = (uint64_t)dst+segment->p_memsz;
                Log("bss: %p\n", p->bss);
                // dirty hack
                end += 4*PG_SIZE;
            }

            alloc_vm(p, start, end, PTE_P|PTE_W|PTE_MATL|PTE_D|PTE_V);
            copy_to_va(p->pgtbl, (uint64_t)vaddr, (char *)src, len);
            // assume all zero
            void *paddr = (void*)va_to_pa(p->pgtbl, (uint64_t)dst);

            if (paddr == NULL) {
                Log("elf loader fail: vaddr %p, paddr %p\n", dst, paddr);
                return -1;
            }

            ldnum++;
        }
    }
    Log("load %p segment(s)\n", ldnum);
    Log("elf entry %p\n", header->e_entry);
    uintptr_t entry = header->e_entry;
    return entry;
}
void exec_helper();
int exec_file(char *path) {
    char *argv[] = {path, NULL};
    char *envp[] = {NULL};
    struct task_struct *p = exec(path, argv, envp);
    if ( !p)
        return -1;
    return 0;
}

struct task_struct *exec(char *path, char *argv[], char *envp[]) {
    struct task_struct *p = proc_alloc();
    p->parent = &proc[0];
    p->state = TASK_RUNABLE;
    if ( cur_proc == &proc[0] ){
        // printf("kproc");
    } else {
        p->fdtable = cur_proc->fdtable;
    }
    
    char *name = (argv[0]) ? argv[0] : path;
    strcpy(p->name, name);

    uintptr_t entry = elf_loader(p, path);
    if (entry == -1) {
        proc_free(p);
        return NULL;
    }

    alloc_vm(p, USER_STACK_BOTTOM, USER_STACK_TOP, PTE_P|PTE_W|PTE_MATL|PTE_D|PTE_V);
    uint64_t new_page = va_to_pa(p->pgtbl, USER_STACK_TOP - PG_SIZE) + PG_SIZE;
    new_page = PA2VA(new_page);
    p->context = (void*)(new_page - sizeof(struct context));

    uint8_t *sp = (uint8_t*)USER_STACK_TOP;
    uint8_t *bp = (uint8_t*)USER_STACK_BOTTOM;
    sp -= sizeof(struct context);
    struct context *context = (void*)sp;

    sp -= sizeof(uint64_t) * (ARGS_MAX+2+ENVS_MAX+1);
    uint64_t argc, len, envc;
    uint64_t *ustack = (void*)sp;
    for (argc=0; argc < ARGS_MAX && argv[argc]; argc++) {
        copy_to_va(p->pgtbl, (uint64_t)&ustack[argc+1], (char*)&bp, sizeof(uintptr_t));
        len = strlen(argv[argc])+1;
        copy_to_va(p->pgtbl, (uint64_t)bp, argv[argc], len);
        bp += len;
    }
    if ( argv[argc])
        panic("exec: so many args");

    for (envc=0; envc < ENVS_MAX && envp[envc]; envc++) {
        copy_to_va(p->pgtbl, (uint64_t)&ustack[argc+2+envc], (char*)&bp, sizeof(uintptr_t));
        len = strlen(envp[envc])+1;
        copy_to_va(p->pgtbl, (uint64_t)bp, envp[envc], len);
        bp += len;
    }
    if ( envp[envc]){
        printf("envc = %d\n", envc);
        printf("&envp[0] = %p\n", &envp[0]);
        printf("envp[0] = %x\n", envp[0]);
        panic("exec: so many envs");
    }
    sp -= sizeof(struct trapframe);
    struct trapframe *trapframe = (void*)sp;

    // printf("p->name: %s\n", p->name);
    uint64_t zero = 0, helper = (uint64_t)&exec_helper, user_sp = (uint64_t)&ustack[0], prmd = 0x4;
    copy_to_va(p->pgtbl, (uint64_t)&ustack[argc+2+envc], (char*)&zero, sizeof(uintptr_t));
    copy_to_va(p->pgtbl, (uint64_t)&ustack[argc+1], (char*)&zero, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&ustack[0], (char*)&argc, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&(context->sp), (char*)&sp, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&(context->ra), (char*)&helper, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&(trapframe->sp), (char*)&user_sp, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&(trapframe->prmd), (char*)&prmd, sizeof(uint64_t));
    copy_to_va(p->pgtbl, (uint64_t)&(trapframe->era), (char*)&entry, sizeof(uint64_t));

    return p;
}

struct task_struct *fork(struct task_struct *p) {
    struct task_struct *np = proc_alloc();
    copy_pagetable(p, np);

    list_add(&p->children, &np->tasks);
    np->parent = p;
    np->state = TASK_RUNABLE;
    np->bss = p->bss;
    strcpy(np->name, p->name);
    // strcat(np->name, "-fork");
    // np->fdtable = fdtable_copy(p->fdtable);
    np->fdtable = p->fdtable;
    strcpy(np->cwd, p->cwd);
    np->callback = p->callback;
    np->timer_call_time = p->timer_call_time;

    struct trapframe *trapframe = get_trapframe();
    struct context *context = (void*)(USER_STACK_TOP - sizeof(struct context));
    void *new_page = (void*)va_to_pa(np->pgtbl, USER_STACK_TOP - PG_SIZE) + PG_SIZE;
    np->context = (void*)(PA2VA(new_page) - sizeof(struct context));
    uint8_t *sp = (void*)trapframe;

    uint64_t zero = 0, helper = (uint64_t)&exec_helper;
    copy_to_va(np->pgtbl, (uint64_t)&(trapframe->a0), (char*)&zero, sizeof(uint64_t));
    copy_to_va(np->pgtbl, (uint64_t)&(context->sp), (char*)&sp, sizeof(uint64_t));
    copy_to_va(np->pgtbl, (uint64_t)&(context->ra), (char*)&helper, sizeof(uint64_t));
    // Log("forking: %p->%p\n", p, np);
    Log("forking: %d->%d\n", p->pid, np->pid);
    // Log("forking: %s->%s\n", p->name, np->name);
    return np;
}
