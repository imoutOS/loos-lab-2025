#ifndef KLIB_H__
#define KLIB_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

// inb cli ...
#include "asm/cpu.h"

#include "kconfig.h"

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
static inline uint32_t ind(uintptr_t addr) { return *(volatile uint32_t *)addr; }
static inline uint64_t inq(uintptr_t addr) { return *(volatile uint64_t *)addr; }

static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outd(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }
static inline void outq(uintptr_t addr, uint64_t data) { *(volatile uint64_t *)addr = data; }

// am.h
void clear_screen();
void putch(char);
void putstr(char*);
int getch();
char *getstr(uint64_t);

// string.h
void  *memset    (void *s, int c, size_t n);
void  *memcpy    (void *dst, const void *src, size_t n);
void  *memmove   (void *dst, const void *src, size_t n);
int    memcmp    (const void *s1, const void *s2, size_t n);
size_t strlen    (const char *s);
char  *strcat    (char *dst, const char *src);
char  *strcpy    (char *dst, const char *src);
char  *strncpy   (char *dst, const char *src, size_t n);
int    strcmp    (const char *s1, const char *s2);
int    strncmp   (const char *s1, const char *s2, size_t n);
int    strcasecmp(const char *s1, const char *s2);
char  *strtok    (char *str, const char *delim);
size_t strspn    (const char *s, const char *accept);
size_t strcspn   (const char *s, const char *reject);

// stdlib.h
void   srand     (unsigned int seed);
int    rand      (void);
void  *malloc    (size_t size);
void   free      (void *ptr);
int    abs       (int x);
int    atoi      (const char *nptr);
int    min       (int a, int b);
int    max       (int a, int b);

// stdio.h
int    printf    (const char *format, ...);
int    sprintf   (char *str, const char *format, ...);
int    snprintf  (char *str, size_t size, const char *format, ...);
int    vsprintf  (char *str, const char *format, va_list ap);
int    vsnprintf (char *str, size_t size, const char *format, va_list ap);

#define ALIGN(val,to) ((val)/(to)*(to))
#define ALIGNUP(val,to) (ALIGN((val)-1,(to))+(to))
#define PG_SIZE (0x4000)

// assert.h
extern void halt(int);

#define LOG_LEVEL_TRACE                 4
#define LOG_LEVEL_DEBUG                 3
#define LOG_LEVEL_INFO                  2
#define LOG_LEVEL_CRITICAL              1
#define LOG_LEVEL_EMERGENCY             0

  /* #define Log(level, ...) \
      // do { \
          // if (level <= LOG_LEVEL) printf(__VA_ARGS__); \
      // } while(0) */
/* define macros according to Kconfig */
#if defined(CONFIG_ENABLE_DEBUG)
  #define Log(...) ((void)0)
  #define assert(cond) \
    do { \
      if (!(cond)) { \
        printf("%s:%d: Assertion `%s` failed\n", __FILE__, __LINE__, #cond); \
        if (CONFIG_ASSERT_HALT_BEHAVIOR) halt(1); \
      } \
    } while (0)
#else
  #define Log(...) ((void)0)
  #define assert(ignore) ((void)0)
#endif

/* panic config */
#define panic(...) \
do { \
  if (IS_ENABLED(CONFIG_FORCE_PANIC_MESSAGES) || IS_ENABLED(CONFIG_ENABLE_DEBUG)) { \
    printf("panic at %s:%s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
  } \
  halt(1); \
  __builtin_unreachable(); \
} while(0)
#endif

#define DEBUG_SYSCALL 0
#define sys_debug(fmt, ...) \
    do { if (DEBUG_SYSCALL) printf("SYSCALL [%s:%d] " fmt, __func__, __LINE__, ##__VA_ARGS__); } while (0)
// Add debug macro (can be enabled/disabled)
#define DEBUG_VFS 0
#define vfs_debug(fmt, ...) \
    do { if (DEBUG_VFS) printf("VFS [%s:%d] (pid=%d) " fmt, __func__, __LINE__, my_proc()->pid, ##__VA_ARGS__); } while (0)
