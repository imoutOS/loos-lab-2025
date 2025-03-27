#include "bits.h"
#include "struct/bitmap.h"
#include "klib.h"

bool bitmap_is_in_use(uint8_t *page_map, size_t map_max, int index) {
    assert(index < map_max);
    return get_bit(page_map[index/8], index%8);
}
void bitmap_set_status(uint8_t *page_map, size_t map_max, int index, bool status) {
    assert(index < map_max);
    page_map[index/8] = (uint8_t)set_bit(page_map[index/8], index%8, status);
}

size_t bitmap_alloc_one(uint8_t *page_map, size_t map_max) {
    uint8_t data;
    int bit = 0;
    for (size_t i=0; i < map_max; i++) {
        data = page_map[i];
        if (data != 0xFF) {
            while (data & 1u) {
                data = data >> 1;
                bit++;
            }
            bitmap_set_status(page_map, map_max, 8*i + bit, 1);
            return 8*i + bit;
        }
    }
    return -1;
}

size_t bitmap_alloc(uint8_t *page_map, size_t map_max, size_t cnt) {
    for (size_t i=0; i < map_max; i++) {
        if ( !bitmap_is_in_use(page_map, map_max, i)) {
            size_t j;
            for (j=0; j < cnt; j++)
                if (bitmap_is_in_use(page_map, map_max, i+j)) 
                    break;
            assert (j <= cnt);
            if (j >= cnt) {
                for (int k=0; k < j; k++)
                    bitmap_set_status(page_map, map_max, i+k, 1);
                return i;
            }
            i += j;
        }
    }
    return -1;
}
void bitmap_free(uint8_t *page_map, size_t map_max, size_t index, size_t cnt) {
    for (size_t i=0; i < cnt; i++) {
        assert (bitmap_is_in_use(page_map, map_max, index+i));
        bitmap_set_status(page_map, map_max, index+i, 0);
    }
}
