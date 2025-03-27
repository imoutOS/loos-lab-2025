#ifndef __BITS_H__
#define __BITS_H__

#include <stdint.h>
#include <stdbool.h>
#include <klib.h>

// start with 0
static inline bool get_bit(uint64_t x, int l) {
    return (x >> l) & 1u;
}
static inline uint64_t set_bit(uint64_t x, int l, bool status) {
    return status ? (x | (1u << l)) : (x & ~(1u << l));
}

static inline uint64_t get_bits_range(uint64_t x, int s, int len) {
    return (x >> s) & ((1ull << len) - 1);
}

static inline void hexdump(uint8_t *buf, int len) {
    printf("hexdump %p 0x%x {\n", buf, len);
    for (int i=0; i < len; i++) {
        printf("%02x ", buf[i]);
        if (i % 16 == 15)
            printf("\n");
    }
    printf("}\n");
}

#endif