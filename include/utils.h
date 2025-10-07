#ifndef UTILS_H
#define UTILS_H

#define ARM_ISB() __asm__ volatile("isb" : : : "memory")

int align_offset(unsigned int i, unsigned int align);

#endif // UTILS_H