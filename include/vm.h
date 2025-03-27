#ifndef VM_H
#define VM_H

struct mapping {
    uint64_t pa;
    uint64_t va;
    uint64_t size;
    uint64_t perm;
};


#endif /* !VM_H */
