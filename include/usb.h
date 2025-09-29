#ifndef USB_H
#define USB_H

#include "types.h"
// From BCM2835 15.USB section
#define USB_BASE        (MMIO_BASE + 0x980000)
#define USB_MDIO_CNTL   (USB_BASE + 0x080)      // MDIO interface control
#define USB_MDIO_GEN    (USB_BASE + 0x084)      // Data for MDIO interface  
#define USB_MDIO_DRV    (USB_BASE + 0x088)
// From https://github.com/rsta2/uspi/blob/master/include/uspi/dwhci.h#L97
// The DWC2 USB controller registers, we need to go to synopsys documentation for details
// (which need a registration to access)
#define DWHCI_CORE_VENDOR_ID (USB_BASE + 0x040)

#define USB_HOST_ID        0x0
#define USB_HCD_ID         0x1
#define USB_OTG_ID         0x2
#define USB_DEVICE_ID      0x3 

#define POWER_STATE_OFF      0x0
#define POWER_STATE_ON       0x1
#define POWER_STATE_WAIT     0x2
/**
 * @brief Initializes the USB controller.
 * 
 * TODO: Implement the necessary logic to initialize the USB controller.
 * This may involve configuring GPIOs, setting up controller registers,
 * and enabling interrupts.
 * For a Raspberry Pi, you'll likely be interacting with the DWC2 USB controller.
 * A good reference is the USPi library (rsta2/uspi on GitHub).
 */
void usb_init();
void PowerOnUSB();
boolean DeviceInitCore();
boolean DeviceInitHost();
boolean DeviceEnableRootPort();
boolean DeviceInitRootPort();

/**
 * @brief Reads a single block from a USB mass storage device.
 * 
 * @param block_lba The logical block address of the block to read.
 * @param buffer A pointer to a buffer to store the read data. The buffer
 *               size should be appropriate for the block size of the device.
 * @return 0 on success, non-zero on failure.
 * 
 * TODO: Implement the logic to send the necessary USB commands (via Bulk-Only Transport)
 *       to read a block from the connected USB storage device.
 *       This involves:
 *       1. Sending a Command Block Wrapper (CBW).
 *       2. Transferring the data from the device to the buffer.
 *       3. Receiving a Command Status Wrapper (CSW).
 */
int read_block(unsigned int block_lba, unsigned char* buffer);

#endif // USB_H
