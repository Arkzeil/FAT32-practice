#include "uart.h"
#include "usb.h"

void usb_init() {
    uart_puts("TODO: Initialize USB...\r\n");
    // TODO: Implement USB initialization logic here.
}

int read_block(unsigned int block_lba, unsigned char* buffer) {
    uart_puts("TODO: Read USB block ");
    // A simple integer to string for debugging
    char lba_str[11];
    int i = 0;
    if (block_lba == 0) {
        lba_str[i++] = '0';
    } else {
        unsigned int n = block_lba;
        while(n > 0) {
            lba_str[i++] = (n % 10) + '0';
            n /= 10;
        }
        // reverse
        for(int j = 0; j < i / 2; j++) {
            char tmp = lba_str[j];
            lba_str[j] = lba_str[i - j - 1];
            lba_str[i - j - 1] = tmp;
        }
    }
    lba_str[i] = '\0';
    uart_puts(lba_str);
    uart_puts("...\r\n");
    
    // Dummy implementation
    for(int j=0; j<512; ++j) buffer[j] = 0;

    return 0;
}
