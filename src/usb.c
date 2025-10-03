#include "gpio.h"
#include "uart.h"
#include "mailbox.h"
#include "timer.h"
#include "usb.h"

void usb_init() {
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

    PowerOnUSB();

    if(DeviceInitCore())
        uart_puts("USB device core initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB device core.\r\n");


    if(DeviceInitHost())
        uart_puts("USB host initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB host.\r\n");

    // delay_ms(1000); // wait for a while

    if(DeviceEnableRootPort())
        uart_puts("USB root port enabled successfully.\r\n");
    else
        uart_puts("Failed to enable USB root port.\r\n");

    if(DeviceInitRootPort())
        uart_puts("USB root port initialized successfully.\r\n");
    else
        uart_puts("Failed to initialize USB root port.\r\n");
}

void PowerOnUSB() {
    // uart_puts("TODO: Power on USB...\r\n");
    // It should use mailbox to set the power state of USB
    set_power(USB_DEVICE_ID, POWER_STATE_ON | POWER_STATE_WAIT);
}

boolean DeviceInitCore() {
    // uart_puts("TODO: Initialize USB device core...\r\n");
    unsigned int USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_ULPI_EXT_VBUS_DRV; // Clear ULPI external VBUS drive
    USBConfig &= ~DWHCI_CORE_USB_CFG_TERM_SEL_DL_PULSE;  // Set Termination select for DL pulse
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    DeviceReset(100); // 1 second timeout

    USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_ULPI_UTMI_SEL; // Select UTMI+PHY interface
    USBConfig &= ~DWHCI_CORE_USB_CFG_PHYIF; // Set PHY interface
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    unsigned int HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2);
    if(DWHCI_CORE_HW_CFG2_ARCHITECTURE(HWConfig2) != 2) { // Check architecture
        uart_puts("USB controller architecture is not supported!\r\n");
        return false;
    }
    HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2); // Read again
    // check PHY types
    if(DWHCI_CORE_HW_CFG2_HS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI_ULPI &&
        DWHCI_CORE_HW_CFG2_FS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_FS_PHY_TYPE_DEDICATED) 
    {
        HWConfig2 |= DWHCI_CORE_USB_CFG_ULPI_FSLS; // Set ULPI FS/LS select
        HWConfig2 |= DWHCI_CORE_USB_CFG_ULPI_CLK_SUS_M; // Set ULPI clock suspendMode
    }
    else{
        HWConfig2 &= ~DWHCI_CORE_USB_CFG_ULPI_FSLS; // Clear ULPI FS/LS select
        HWConfig2 &= ~DWHCI_CORE_USB_CFG_ULPI_CLK_SUS_M; // Clear ULPI clock suspendMode
    }
    mmio_write(DWHCI_CORE_USB_CFG, HWConfig2);

    unsigned int AHBConfig = mmio_read(DWHCI_CORE_AHB_CFG);
    AHBConfig |= DWHCI_CORE_AHB_CFG_DMAENABLE;            // Enable DMA
    AHBConfig |= DWHCI_CORE_AHB_CFG_WAIT_AXI_WRITES;      // Wait for AXI writes
    AHBConfig &= ~DWHCI_CORE_AHB_CFG_MAX_AXI_BURST__MASK; // Clear max AXI burst
    mmio_write(DWHCI_CORE_AHB_CFG, AHBConfig);

    USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_HNP_CAPABLE; // Clear HNP capable
    USBConfig &= ~DWHCI_CORE_USB_CFG_SRP_CAPABLE; // Clear SRP capable
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    return true;
}

boolean DeviceInitHost() {
    // uart_puts("TODO: Initialize USB host...\r\n");
    // restart PHY clock
    unsigned int Power = mmio_read(USB_POWER);
    Power = 0; // Set to 0 to restart PHY clock
    mmio_write(USB_POWER, Power);

    unsigned int HostConfig = mmio_read(DWHCI_HOST_CFG);
    HostConfig &= ~DWHCI_HOST_CFG_FSLS_PCLK_SEL__MASK; // Set FS/LS PCLK select to 0 (48MHz)

    unsigned int HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2);
    unsigned int USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    // set FS/LS PCLK select based on PHY types
    if(DWHCI_CORE_HW_CFG2_HS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI_ULPI &&
        DWHCI_CORE_HW_CFG2_FS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_FS_PHY_TYPE_DEDICATED &&
        (USBConfig & DWHCI_CORE_USB_CFG_ULPI_FSLS) )
    {
        HostConfig |= DWHCI_HOST_CFG_FSLS_PCLK_SEL_48_MHZ; // Set FS/LS PCLK select to 1 (48MHz)
    }
    else{
        HostConfig |= DWHCI_HOST_CFG_FSLS_PCLK_SEL_30_60_MHZ; // Set FS/LS PCLK select to 0 (30-60MHz)
    }
    mmio_write(DWHCI_HOST_CFG, HostConfig);

    // assume dynamic FIFO sizing is not supported, so we skip that part

    // flush all Tx FIFO
    unsigned int reset = mmio_read(DWHCI_CORE_RESET);
    reset |= DWHCI_CORE_RESET_TX_FIFO_FLUSH;
    reset &= ~DWHCI_CORE_RESET_TX_FIFO_NUM__MASK;
    reset |= (0x10 << DWHCI_CORE_RESET_TX_FIFO_NUM__SHIFT); // Flush
    mmio_write(DWHCI_CORE_RESET, reset);
    DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_TX_FIFO_FLUSH, 0, 1000000);

    // flush the Rx FIFO
    reset = mmio_read(DWHCI_CORE_RESET);
    reset |= DWHCI_CORE_RESET_RX_FIFO_FLUSH;
    mmio_write(DWHCI_CORE_RESET, reset);
    DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_RX_FIFO_FLUSH, 0, 1000000);

    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port control and status
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort = mmio_read(DWHCI_HOST_PORT); // Read again to clear
    if(!(HostPort & DWHCI_HOST_PORT_POWER)) {
        HostPort |= DWHCI_HOST_PORT_POWER; // Set port power
        mmio_write(DWHCI_HOST_PORT, HostPort);
    }

    return true;
}

boolean DeviceEnableRootPort() {
    // uart_puts("TODO: Enable USB root port...\r\n");
    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port control and status
    if(!DeviceWaitForBit((unsigned int*)DWHCI_HOST_PORT, DWHCI_HOST_PORT_CONNECT, 1, 1000000)) 
        return false;
    
    delay_ms(100); // USB 2.0 spec requires at least 100ms delay before enabling the port

    HostPort = mmio_read(DWHCI_HOST_PORT);      // Read again
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort |= DWHCI_HOST_PORT_ENABLE;         // Set port enable
    mmio_write(DWHCI_HOST_PORT, HostPort);

    delay_ms(50); // USB 2.0 spec

    HostPort = mmio_read(DWHCI_HOST_PORT);      // Read again
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort &= ~DWHCI_HOST_PORT_RESET;         // Clear port reset
    mmio_write(DWHCI_HOST_PORT, HostPort);

    delay_ms(20);                               // Usually 10-20ms is enough

    return true;
}

boolean DeviceInitRootPort() {
    // uart_puts("TODO: Initialize USB root port...\r\n");
    USBSpeed speed = DeviceGetPortSpeed();
    if(speed == USBSpeedUnknown) {
        uart_puts("Unknown USB speed detected!\r\n");
        return false;
    }

    
    return true;
}

USBSpeed DeviceGetPortSpeed() {
    USBSpeed speed = USBSpeedUnknown;

    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port

    switch(DWHCI_HOST_PORT_SPEED(HostPort)) {
        case DWHCI_HOST_PORT_SPEED_HIGH:
            speed = USBSpeedHigh;
            break;
        case DWHCI_HOST_PORT_SPEED_FULL:
            speed = USBSpeedFull;
            break;
        case DWHCI_HOST_PORT_SPEED_LOW:
            speed = USBSpeedLow;
            break;
        default:
            break;
    }

    return speed;
}

boolean DeviceReset(unsigned int timeout) {
    // uart_puts("TODO: Reset USB device...\r\n");

    // wait for AHB master IDLE
    if(!DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_AHB_IDLE, 1, timeout)) {
        uart_puts("Timeout waiting for AHB master IDLE\r\n");
        return false;
    }
    // set core soft reset
    mmio_write(DWHCI_CORE_RESET, 1);

    // wait for core soft reset to complete
    if(!DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, 1, 0, timeout)) {
        uart_puts("Timeout waiting for core soft reset to complete\r\n");
        return false;
    }

    return true;
}

boolean DeviceWaitForBit(unsigned int* reg, unsigned int bit, unsigned int value, unsigned int timeout) {
    // need '?1:0' as directly take the result is not guaranteed to be 0 or 1
    while(((mmio_read((long)reg) & bit) ? 1 : 0) != value) {
        delay_ms(1);

        timeout--;
        if(timeout == 0) {
            uart_puts("Timeout waiting for bit\r\n");
            uart_puts("\r\n");
            return false;
        }
    }
    // if the value inside reg is unsigned int and its 31st bit is 1, it will be negative if directly cast to int
    // unsigned int reg_val = mmio_read((long)reg);
    // uart_itoa(reg_val);
    // uart_puts("\r\n");
    return true;
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
