#ifndef __STRUCT_BITMAP_H__
#define __STRUCT_BITMAP_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t bitmap_t[0x1000];
//typedef uint8_t* bitmap_t;

bool bitmap_is_in_use(uint8_t *page_map, size_t map_max, int index);
void bitmap_set_status(uint8_t *page_map, size_t map_max, int index, bool status);
size_t bitmap_alloc_one(uint8_t *page_map, size_t map_max);

size_t bitmap_alloc(uint8_t *page_map, size_t map_max, size_t cnt);
void bitmap_free(uint8_t *page_map, size_t map_max, size_t index, size_t cnt);

#endif
