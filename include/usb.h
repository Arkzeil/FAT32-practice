#ifndef USB_H
#define USB_H

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
