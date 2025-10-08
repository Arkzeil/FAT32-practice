#include "gpio.h"
#include "uart.h"
#include "mailbox.h"
#include "timer.h"
#include "usb.h"
#include "dwcdevice.h"

void usb_init(void) {
    uart_puts("TODO: Initialize USB...\r\n");
    // TODO: Implement USB initialization logic here.
    if(mmio_read(DWHCI_CORE_VENDOR_ID) == 0x4F54280A) {
        uart_puts("DWC2 USB controller detected.\r\n");
    } else {
        uart_puts("No DWC2 USB controller detected!\r\n");
        uart_puts("Vendor ID: ");
        uart_itoa(mmio_read(DWHCI_CORE_VENDOR_ID));
        uart_puts("\r\n");
    }

    DWCDevice dev;

    PowerOnUSB();

    if(DeviceInitCore(&dev))
        uart_puts("USB device core initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB device core.\r\n");

    if(DeviceInitHost(&dev))
        uart_puts("USB host initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB host.\r\n");

    // delay_ms(1000); // wait for a while

    if(DeviceEnableRootPort(&dev))
        uart_puts("USB root port enabled successfully.\r\n");
    else
        uart_puts("Failed to enable USB root port.\r\n");

    if(DeviceInitRootPort(&(dev.m_RootPort)))
        uart_puts("USB root port initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB root port.\r\n");
}

void PowerOnUSB() {
    // uart_puts("TODO: Power on USB...\r\n");
    // It should use mailbox to set the power state of USB
    set_power(USB_DEVICE_ID, POWER_STATE_ON | POWER_STATE_WAIT);
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
