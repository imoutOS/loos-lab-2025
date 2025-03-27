#include "klib.h"
#define MAX_STR_LEN 1024

size_t strlen(const char *s) {
    const char *p = s;
    while (*s) s++;
    return s-p;
}

char *strcpy(char *dst, const char *src) {
    //return strncpy(dst, src, strlen(src)+1);
    char *res = dst;
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
    return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *res = dst;
    while (*src && n) {
        *dst++ = *src++;
        n--;
    }
    while (n) {
        *dst++ = '\0';
        n--;
    }
    return res;
}

char *strcat(char *dst, const char *src) {
    char *res = dst;
    while (*dst) dst++;
    while (*src) *dst++ = *src++;
    *dst = '\0';
    return res;
}

int strcmp(const char *s1, const char *s2) {
    return strncmp(s1, s2, MAX_STR_LEN);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while ((*s1 != '\0') && (*s1 == *s2) && n>1) {
        s1++; s2++;
        n--;
    }
    return *s1 - *s2;
}

void *memset(void *s, int c, size_t n) {
    while (n--)
        *(uint8_t*)s++ = (uint8_t)c;
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    uint8_t *out = dst;
    const uint8_t *in = src;
    if (src < dst && dst <= src+n) {
        out = dst+n-1;
        in = src+n-1;
        while (n--)
            *(uint8_t*)out-- = *(uint8_t*)in--;
    } else
        memcpy(out, in, n);
    return out;
}

void *memcpy(void *out, const void *in, size_t n) {
    while (n--)
        *(uint8_t*)out++ = *(uint8_t*)in++;
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *m1=s1, *m2=s2;
    while ((*m1 == *m2) && n>1) {
        m1++; m2++; n--;
    }
    return *m1 - *m2;
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 'a' - 'A';
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 'a' - 'A';
        }
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return (*s1 - *s2);
}

char *strtok(char *str, const char *delim) {
    static char *lasts;
    char *sbegin, *send;

    sbegin = str ? str : lasts;
    if (!sbegin) {
        return NULL;
    }

    sbegin += strspn(sbegin, delim);
    if (*sbegin == '\0') {
        lasts = NULL;
        return NULL;
    }

    send = sbegin + strcspn(sbegin, delim);
    if (*send != '\0') {
        *send++ = '\0';
    }
    lasts = send;

    return sbegin;
}

size_t strspn(const char *s, const char *accept) {
    const char *p;
    const char *a;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a) {
                break;
            }
        }
        if (*a == '\0') {
            return count;
        }
        ++count;
    }
    return count;
}

size_t strcspn(const char *s, const char *reject) {
    const char *p;
    const char *r;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (r = reject; *r != '\0'; ++r) {
            if (*p == *r) {
                return count;
            }
        }
        ++count;
    }
    return count;
}

// without fully test
void qsort(void *base, size_t num, size_t size, int (*compar)(const void *, const void *)) {
    if (num <= 1) {
        return; // 如果元素个数小于等于1，无需排序
    }

    // 使用第一个元素作为基准值
    void *pivot = base;
    void *left = base + size;
    void *right = base + (num - 1) * size;

    while (left <= right) {
        // 找到左边第一个大于基准值的元素
        while (left <= right && compar(left, pivot) <= 0) {
            left += size;
        }
        // 找到右边第一个小于基准值的元素
        while (left <= right && compar(right, pivot) > 0) {
            right -= size;
        }
        // 交换这两个元素
        if (left < right) {
            void *temp = malloc(size);
            memcpy(temp, left, size);
            memcpy(left, right, size);
            memcpy(right, temp, size);
            free(temp);

            left += size;
            right -= size;
        }
    }
    // 将基准值移到正确的位置
    void *temp = malloc(size);
    memcpy(temp, pivot, size);
    memcpy(pivot, right, size);
    memcpy(right, temp, size);
    free(temp);

    // 递归排序左右两部分
    size_t left_size = (right - base) / size;
    size_t right_size = num - left_size - 1;
    qsort(base, left_size, size, compar);
    qsort(right + size, right_size, size, compar);
}
