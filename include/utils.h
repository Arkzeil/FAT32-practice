#ifndef UTILS_H
#define UTILS_H

#include "uart.h"

#define STRINGIFY(x) #x
#define ARM_ISB() __asm__ volatile("isb" : : : "memory")

#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            uart_puts("Assertion failed: " #condition " at " __FILE__ ":" STRINGIFY(__LINE__) "\r\n");  \
        } \
    } while (0)

int align_offset(unsigned int i, unsigned int align);
int int_to_hex(unsigned int value, char *buffer, int buffer_size);

#endif // UTILS_H