#ifndef __STRUCT_MAP_H__
#define __STRUCT_MAP_H__

#include <stdint.h>

struct map_item {
    uintptr_t key;
    uintptr_t value;
};
struct map {
    uint8_t len;
    struct map_item item[16];
};

void map_add(struct map *map, uintptr_t key, uintptr_t value);
uintptr_t map_find(struct map *map, uintptr_t key);
void map_delete(struct map *map, uintptr_t key);
struct map_item *map_pop(struct map *map);
void map_print(struct map *map);

#endif
