#include "klib.h"
#include "vm.h"
#include "memlayout.h"

//2k1000
// const unsigned long long uart_base = 0x800000001fe20000ULL;
// 2k500
//static unsigned long uart_base = 0x800000001ff40800ULL;

#define _min(a,b) ( ((a)<(b)) ? (a) : (b))

void putch(char c) {
    // wait for Transmit Holding Empty to be set in LSR.
    while ((inb(UART0_LSR) & LSR_TX_IDLE) == 0)
        ;
    outb(UART0_THR, c);
}

void putstr(char *s) {
    while (*s) {
        putch(*s);
        s++;
    }
}

int getch() {
    if (inb(UART0_LSR) & 0x1) {
        int key = inb(UART0_RHR);
        return (key == 13) ? '\n' : key;
    } else {
        // no data
        return -1;
    }
}

char getstr_buf[128];
char *getstr(uint64_t count) {
    int c = 0;
    char *p = getstr_buf;
    count = _min(count, sizeof(getstr_buf));
    while (count && ((c = getch()) != '\n')) {
        if (c == -1) {
            intr_on();
            for (int i=0; i < 100; i++)
                ;
            intr_off();
            continue;
        }
        *p++ = (char)c;
        if (count > 1)
            putch(c);
        count--;
    }
    if (c == '\n') {
        *p++ = '\n';
        if (count > 1)
            putch('\n');
    }
    *p = '\0';
    return getstr_buf;
}
