#include "uart.h"
#include "my_bare_usb.h"

void main() {
    uart_init();

    uart_puts("----------------------------------\r\n");
    uart_puts("Welcome to FAT32 Practice Kernel\r\n");
    uart_puts("----------------------------------\r\n");

    // Call the low-level USB practice function
    practice_usb_from_scratch();

    uart_puts("Initialization complete. Hanging.\r\n");
}