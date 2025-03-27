#include "klib.h"
#include "struct/map.h"

void map_add(struct map *map, uintptr_t key, uintptr_t value) {
    map->item[map->len].key = key;
    map->item[map->len].value = value;
    map->len++;
}

uintptr_t map_find(struct map *map, uintptr_t key) {
    for (uint8_t i=0; i < map->len; i++) {
        if (map->item[i].key == key)
            return map->item[i].value;
    }
    return -1;
}

void map_delete(struct map *map, uintptr_t key) {
    assert(0);
}

struct map_item *map_pop(struct map *map) {
    if (map->len) {
        map->len--;
        return &(map->item[map->len]);
    }
    return (void*)-1;
}

void map_print(struct map *map) {
    printf("map(%d) {\n", map->len);
    for (int i=0; i < map->len; i++) {
        printf("  %p: %p\n", map->item[i].key, map->item[i].value);
    }
    printf("}\n");
}
