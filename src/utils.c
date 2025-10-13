#include "utils.h"
#include "types.h"

int align_offset(unsigned int i, unsigned int align) {
    if(align == 0) return i; // avoid division by zero
    return ((i + align - 1) & ~(align - 1)) - i;
}

int int_to_hex(unsigned int value, char *buffer, int buffer_size) {
    if (buffer_size < 3) { // Need at least space for 1 byte data and null terminator
        if (buffer_size > 0) {
            buffer[0] = '\0'; // Empty string if not enough space
        }
        return 0;
    }

    const char hex_chars[] = "0123456789ABCDEF";
    int pos = 0;

    // buffer[pos++] = '0';
    // buffer[pos++] = 'x';

    boolean started = false; // Flag to indicate if we've started adding digits
    for (int shift = (sizeof(value) * 8) - 4; shift >= 0; shift -= 4) {
        unsigned int nibble = (value >> shift) & 0xF;
        if (nibble != 0 || started || shift == 0) { // Always add last digit
            if (pos < buffer_size - 1) { // Leave space for null terminator
                buffer[pos++] = hex_chars[nibble];
                started = true;
            } else {
                break; // No more space in buffer
            }
        }
    }

    buffer[pos] = '\0'; // Null-terminate the string
    return pos; // Return the length of the string
}