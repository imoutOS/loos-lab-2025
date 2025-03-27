#include "klib.h"
#include "mm.h"

static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}


extern char _heap_start, _heap_end;
uint8_t *hbrk = (uint8_t*)&_heap_start;

struct fastbin_chunk {
    struct fastbin_chunk *next;
    size_t size;
    uint8_t data[0];
};
#define chunk_info_size ( sizeof(struct fastbin_chunk*) + sizeof(size_t) )
struct fastbin_chunk *fastbin[11] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
struct fastbin_chunk *unsorted_bin = NULL;

// void *apply_alloc(size_t size) {
//     uint8_t *victim = hbrk;
//     hbrk += size;
//     if (hbrk >= (uint8_t*)&_heap_end) {
//         panic("apply alloc");
//         return NULL;
//     }
//     // printf("apply %p\n", hbrk);
//     return victim;
// }

uint8_t *p_start = NULL,
    *p_end = NULL;
void *apply_alloc(size_t size) {
    if (size > PG_SIZE) {
        panic("apply");
    }
    if (p_start + size >= p_end) {
        p_start = kmalloc(PG_SIZE);
        p_end = p_start + PG_SIZE;
    }
    void *victim = p_start;
    p_start += size;
    return victim;
}

// not thread safe
void *malloc(size_t size) {
    struct fastbin_chunk *chunk = NULL;
    if (size >= 0xa0) {
        // unsorted bin
        chunk = unsorted_bin;
        for (int i = 0; chunk && i < 100; chunk = chunk->next, i++)
            if (chunk->size >= size && chunk->size < size + 128) {
                unsorted_bin = unsorted_bin->next;
                break;
            }
        if (!chunk || !(chunk->size >= size && chunk->size < size + 128)) {
            chunk = (struct fastbin_chunk *)apply_alloc(size + chunk_info_size);
        }
    } else {
        // fastbin
        size_t index = size / 0xF;
        chunk = fastbin[index];
        if (chunk) {
            fastbin[index] = fastbin[index]->next;
        } else {
            size_t fix_size = (size + 0xF) & ~0xF;
            chunk = (struct fastbin_chunk *)apply_alloc(fix_size + chunk_info_size);
        }
    }
    if (!chunk)
        return NULL;
    chunk->size = size;
    return chunk->data;
}

void free(void *ptr) {
    struct fastbin_chunk *chunk = (struct fastbin_chunk *)(ptr - chunk_info_size);
    if (chunk->size >= 0xa0) {
        chunk->next = unsorted_bin;
        unsorted_bin = chunk;
    } else {
        size_t index = chunk->size / 0xF;
        chunk->next = fastbin[index];
        fastbin[index] = chunk;
    }
}

void *calloc(size_t nitems, size_t size) {
    void *victim = (void*)malloc(nitems * size);
    if (victim) {
        memset(victim, 0, nitems * size);
    }
    return victim;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}
