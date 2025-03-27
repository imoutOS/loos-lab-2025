#include "klib.h"

#define _min(a,b) ( ((a)<(b)) ? (a) : (b))
#define _max(a,b) ( ((a)>(b)) ? (a) : (b))

typedef uint64_t uint_t;
typedef int64_t int_t;

#define panic_on(cond, msg) \
do { \
  if ((cond)) { \
    putstr(msg); \
    halt(1); \
  } \
} while(0)

#define to_upper(c) ((c>='a' && c<='z') ? c-'a'+'A': c)

const char _itoa_table[] = "0123456789ABCDEF";
// exclude '\0'
int get_itoa_table_index(const char ch, int len) {
    int max_len = _min(len, sizeof(_itoa_table)/sizeof(_itoa_table[0]) - 1);
    for (int i=0; i < max_len; i++)
        if (ch == _itoa_table[i])
            return i;
    return -1;
}

int _uitoa(uint_t value, char *string, int radix) {
    char *pre, *dst, *last = NULL, tmp;
    panic_on(radix != 10 && radix != 16, "this radix not support");
    if (value == 0) {
        *string++ = '0'; *string='\0';
        return 2;
    }
    pre = dst = string;
    while (value) {
        *dst++ = _itoa_table[value % radix];
        value /= radix;
    }
    last = dst-1;
    while (last > pre) {
        tmp = *pre;
        *pre = *last;
        *last = tmp;
        pre++; last--;
    }
    *dst++ = '\0';
    return dst - string;
}

int _itoa(int_t value, char *string, int radix) {
    int is_negative = 0;
    panic_on(radix != 10, "this radix not support");
    if (value < 0) {
        *string++ = '-';
        is_negative = 1;
        value = -value;  // INT_MIN overflow
    }
    return is_negative + _uitoa(value, string, radix);
}

int _uatoi(const char *string, int radix, uint_t *result) {
    const char *s = string;
    uint_t value = 0;
    int bit_value;
    panic_on(radix != 10 && radix != 16, "this radix not support");
    while ((bit_value = get_itoa_table_index(to_upper(*s), radix)) >= 0) {
        value *= radix;
        value += bit_value;
        s++;
    }
    *result = value;
    return s - string;
}

enum {
    NOT_FOUND = -1, FLAG_NOP = 0, FLAG_ZERO, FLAG_LEFT
};

int get_fmt_flag(const char *fmt, int *flag) {
    switch(*fmt) {
        case '+': case ' ': case '#':
            panic("format flag not implemented");
        case '0':
            *flag = FLAG_ZERO;
            return 1;
        case '-':
            *flag = FLAG_LEFT;
            return 1;
        default:
            *flag = FLAG_NOP;
            return 0;
    }
}

int copy_fmt_str(char *dst, const char *buf, int flag, int num) {
    int buf_str_len = strlen(buf),
        len = _max(buf_str_len, num),
        remain = len - buf_str_len;
    switch(flag) {
        case FLAG_NOP: case FLAG_ZERO:
            while (remain-- > 0)
               *dst++ = (flag == FLAG_ZERO) ? '0': ' ';
            strcpy(dst, buf);
            break;
        case FLAG_LEFT:
            strcpy(dst, buf);
            dst += strlen(buf);
            while (remain-- > 0)
                *dst++ = ' ';
            break;
        default:
            panic("Not implemented");
    }
    return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    char c, *dst = out;
    char buf[512];  // stack overflow
    while ((c = *fmt) != '\0') {
        if (c == '%') {
            fmt++;
            int fmt_flag;
            uint_t fmt_width;
            fmt += get_fmt_flag(fmt, &fmt_flag);
            fmt += _uatoi(fmt, 10, &fmt_width);
            switch(*fmt) {
                case 'c': {
                    // 'char' is promoted to 'int' when passed through '...'
                    char ch = va_arg(ap, int);
                    buf[0] = ch; buf[1] = '\0';
                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);
                    if (s) {
                        strcpy(buf, s);
                    } else {
                        strcpy(buf, "(null)");
                    }
                    break;
                }
                case 'd': {
                    int_t d = va_arg(ap, int_t);
                    _itoa(d, buf, 10);
                    break;
                }
                case 'p': // regard p as x
                    *dst++ = '0'; *dst++ = 'x';
                case 'u': case 'x': {
                    uint_t u = va_arg(ap, uint_t);
                    _uitoa(u, buf, (c=='u') ? 10 : 16);
                    break;
                }
                default: {
                    char wrong_ch[4];
                    putstr("vsprintf: unknown format character: ");
                    wrong_ch[0] = *fmt;
                    wrong_ch[1] = '\n';
                    wrong_ch[2] = '\0';
                    panic_on(1, wrong_ch);
                }
            }
            dst += copy_fmt_str(dst, buf, fmt_flag, fmt_width);
        }
        else {
            *dst++ = c;
        }
        fmt++;
    }
    *dst++ = '\0';
    return dst - out;
}

int sprintf(char *out, const char *fmt, ...) {
    int result;
    va_list ap;
    va_start(ap, fmt);
    result = vsprintf(out, fmt, ap);
    va_end(ap);
    return result;
}

int printf(const char *fmt, ...) {
    int result;
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    result = vsprintf(buf, fmt, ap);
    va_end(ap);
    putstr(buf);
    return result;
}

