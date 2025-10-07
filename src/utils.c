#include "utils.h"

int align_offset(unsigned int i, unsigned int align) {
    if(align == 0) return i; // avoid division by zero
    return (i + align - 1) & ~(align - 1) - i;
}