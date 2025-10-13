#include "my_bare_usb.h"
#include "uart.h"
#include "timer.h"
#include "allocator.h"
#include "utils.h"
#include "usb.h"
#include "dwcdevice.h"
#include "gpu.h"

// Base address for the DWC2 USB Controller on Raspberry Pi 3
// It's the same as the USB peripheral base address (USB_BASE) in usb.h
#define DWC2_REGS_BASE 0x3F980000

// Helper to create a volatile pointer to a register
#define DWC2_REG(offset) (*(volatile unsigned int*)(DWC2_REGS_BASE + offset))

// --- Global Registers (Offsets from DWC2_REGS_BASE) ---
#define GAHBCFG     DWC2_REG(0x08)
#define GUSBCFG     DWC2_REG(0x0C)
#define GRSTCTL     DWC2_REG(0x10)
#define GINTSTS     DWC2_REG(0x14)
#define GINTMSK     DWC2_REG(0x18)
#define GRXFSIZ     DWC2_REG(0x24)
#define GNPTXFSIZ   DWC2_REG(0x28)
#define GSNPSID     DWC2_REG(0x40)
#define GHWCFG1     DWC2_REG(0x44)
#define GHWCFG2     DWC2_REG(0x48)
#define GHWCFG3     DWC2_REG(0x4C)
#define GHWCFG4     DWC2_REG(0x50)

// --- Host Mode Registers (Offsets from DWC2_REGS_BASE) ---
#define HCFG        DWC2_REG(0x400)
#define HPRT        DWC2_REG(0x440)

// --- Host Channel Registers (Offsets from DWC2_REGS_BASE) ---
// Note: These are arrays. You access channel 'n' with base + n*0x20
#define HCCHAR(n)   DWC2_REG(0x500 + (n * 0x20))
#define HCSPLT(n)   DWC2_REG(0x504 + (n * 0x20))
#define HCINT(n)    DWC2_REG(0x508 + (n * 0x20))
#define HCINTMSK(n) DWC2_REG(0x50C + (n * 0x20))
#define HCTSIZ(n)   DWC2_REG(0x510 + (n * 0x20))
#define HCDMA(n)    DWC2_REG(0x514 + (n * 0x20))


void practice_usb_from_scratch() {
    uart_puts("--- Starting USB practice from scratch ---\\r\n");

    // --- Step 1: Controller Initialization ---
    uart_puts("1. Initializing DWC2 Controller in Host Mode...\r\n");

    // Set Host Mode
    unsigned int usbcfg = GUSBCFG;
    usbcfg &= ~(1 << 10); // HNPCapable = 0, Host negotiation protocol disabled
    usbcfg &= ~(1 << 9);  // SRPCapable = 0, Session request protocol disabled
    usbcfg |= (1 << 29); // Force Host Mode (Force Dev mode is bit 30)
    GUSBCFG = usbcfg;
    delay_ms(10); // Wait for mode change

    // Enable Global Interrupt Mask in GAHBCFG
    GAHBCFG |= (1 << 0); // Global Interrupt Mask

    // Unmask Host-related interrupts in GINTMSK
    GINTMSK |= (1 << 25); // Host Channels Interrupt Mask
    GINTMSK |= (1 << 24); // Port Interrupt Mask

    uart_puts("   Controller initialized.\r\n");

    // --- Step 2: Port Initialization and Device Detection ---
    uart_puts("2. Resetting Port and detecting device...\r\n");

    // Read port status
    unsigned int hprt_val = HPRT;
    if ((hprt_val & 0x1) == 0) {
        uart_puts("   No device connected.\r\n");
        return;
    }
    uart_puts("   Device connected. Resetting port...\r\n");

    // Reset the port
    HPRT = hprt_val | (1 << 8); // Port Reset
    delay_ms(50); // Hold reset for ~50ms
    HPRT = HPRT & ~(1 << 8);    // Clear Port Reset
    delay_ms(100); // Wait for device to stabilize

    // Read port speed
    hprt_val = HPRT;
    unsigned int speed = (hprt_val >> 17) & 0x3;
    if (speed == USBSpeedHigh) uart_puts("   Device is High-speed.\r\n");
    else if (speed == USBSpeedFull) uart_puts("   Device is Full-speed.\r\n");
    else if (speed == USBSpeedLow) uart_puts("   Device is Low-speed.\r\n");
    else uart_puts("   Speed detection failed.\r\n");

    // --- Step 3: Your first Control Transfer ---
    uart_puts("3. TODO: Perform a Control Transfer (Get Descriptor).\r\n");

    // This is your main task. You will need to:
    // a. Create an 8-byte SETUP packet in memory for GET_DESCRIPTOR.
    // b. Create a buffer in memory to receive the 8 bytes of the descriptor.
    // c. Pick a channel (e.g., channel 0).
    // d. Program HCCHAR0, HCTSIZ0, HCDMA0 for the SETUP stage.
    // e. Enable the channel and poll on HCINT0 until it halts.
    // f. Check the result in HCINT0.
    // g. Repeat for the DATA stage (IN direction).
    // h. Repeat for the STATUS stage (OUT direction, zero length).

    void *setup_packet = simple_malloc(8);
    void *data_buffer = simple_malloc(8);
    ASSERT(setup_packet != NULL && data_buffer != NULL);

    // Fill in the SETUP packet for GET_DESCRIPTOR (Device)
    __attribute__((aligned(4))) unsigned char *setup = (unsigned char *)setup_packet;
    setup[0] = 0x80; // bmRequestType: Device-to-host, Standard, Device (0b 1 00 00000)
    setup[1] = 0x06; // bRequest: GET_DESCRIPTOR
    setup[2] = 0x00; // wValueL (Descriptor Index): 0
    setup[3] = 0x01; // wValueH (Descriptor Type): Device Descriptor
    setup[4] = 0x00; // wIndexL: Language ID (0 for Device)
    setup[5] = 0x00; // wIndexH
    setup[6] = 0x08; // wLengthL: 8 bytes
    setup[7] = 0x00; // wLengthH

    unsigned int channel = 0; // Using channel 0

    // --- SETUP Stage ---
    uart_puts("   Performing SETUP stage...\r\n");
    HCCHAR(channel) = // (0 << 31) | // MPS: 64
                    //   (0 << 29) | // EPTYP: Control
                    //   (0 << 27) | // DAD: 0 (default address)
                    //   (0 << 26) | // MC: 0
                    //   (0 << 25) | // EP: 0
                    //   (1 << 24) | // LSDEV: Low-speed device (change if needed)
                      (0 << DWHCI_HOST_CHAN_CHARACTER_DEVICE_ADDRESS__SHIFT) | // DevAddr: 0
                    //   (0 << 21) | // OddFrame: 0
                    //   (0 << 20) |  // ChDis: 0 (enable channel)
                      (1 << DWHCI_HOST_CHAN_CHARACTER_MULTI_CNT__SHIFT) | // MultiCount: 1
                      (0 << DWHCI_HOST_CHAN_CHARACTER_EP_TYPE__SHIFT) | // EP Type: Control
                      (0 << DWHCI_HOST_CHAN_CHARACTER_EP_NUMBER__SHIFT) & // EP Number: 0
                      ~(DWHCI_HOST_CHAN_CHARACTER_EP_DIRECTION_IN); // EP Direction: OUT (for SETUP)
    if(speed == USBSpeedLow)
        HCCHAR(channel) |= DWHCI_HOST_CHAN_CHARACTER_LOW_SPEED_DEVICE; // Low-speed device
    else
        HCCHAR(channel) &= ~DWHCI_HOST_CHAN_CHARACTER_LOW_SPEED_DEVICE; // Full/High-speed device

    HCTSIZ(channel) = (1 << DWHCI_HOST_CHAN_XFER_SIZ_PID__SHIFT) | // PID: SETUP
                      ((1 << DWHCI_HOST_CHAN_XFER_SIZ_PACKETS__SHIFT) & DWHCI_HOST_CHAN_XFER_SIZ_PACKETS__MASK) | // Packets: 1
                      (8 & DWHCI_HOST_CHAN_XFER_SIZ_BYTES__MASK);  // XferSize: 8 bytes
    HCDMA(channel) = BUS_ADDRESS((unsigned int)(unsigned int*)setup_packet); // DMA address
    uart_b2x(HCCHAR(channel));
    ARM_ISB(); // ensure all previous memory accesses are completed before continuing

    HCCHAR(channel) |= DWHCI_HOST_CHAN_CHARACTER_ENABLE; // Enable channel
    HCCHAR(channel) &= ~DWHCI_HOST_CHAN_CHARACTER_DISABLE; // Clear disable bit
    uart_b2x(HCCHAR(channel));
    // Poll for completion
    while ((HCINT(channel) & (1 << 0)) == 0); // Wait for Transfer Complete
    unsigned int hcint = HCINT(channel);
    HCINT(channel) = hcint; // Clear interrupt
    if (hcint & (1 << 3)) {
        uart_puts("   SETUP stage failed.\r\n");
        return;
    }
    uart_puts("   SETUP stage completed.\r\n");
    // --- DATA Stage ---
    uart_puts("   Performing DATA stage...\r\n");
    HCCHAR(channel) = (0 << 31) | // MPS: 64
                      (0 << 29) | // EPTYP: Control
                      (0 << 27) | // DAD: 0 (default address)
                      (0 << 26) | // MC: 0
                      (0 << 25) | // EP: 0
                      (1 << 24) | // LSDEV: Low-speed device (change if needed)
                      (0 << 22) | // DevAddr: 0
                      (0 << 21) | // OddFrame: 0
                      (0 << 20);  // ChDis: 0 (enable channel)
    // Gemini says that regarding the size of data stage, there seemed usually to be 2 stages:
    // one with 8 bytes to get the actual size of descriptor,
    // then another with the actual size (e.g., 18 bytes for device descriptor)
    HCTSIZ(channel) = (1 << 19) | // PID: IN
                      (USB_DEFAULT_MAX_PACKET_SIZE << 0);   // XferSize: 8 bytes
    HCDMA(channel) = (unsigned int)data_buffer; // DMA address
    HCCHAR(channel) |= (1 << 31); // Enable channel
    // Poll for completion
    while ((HCINT(channel) & (1 << 0)) == 0); // Wait for Transfer Complete
    hcint = HCINT(channel);
    HCINT(channel) = hcint; // Clear interrupt
    if (hcint & (1 << 3)) {
        uart_puts("   DATA stage failed.\r\n");
        return;
    }
    uart_puts("   DATA stage completed. Received Descriptor:\r\n");
    unsigned char *desc = (unsigned char *)data_buffer;
    uart_puts("0x");
    for (int i = 0; i < 8; i++) {
        char buf[4];
        int len = int_to_hex((int)desc[i], buf, sizeof(buf));
        buf[len] = ' ';
        buf[len + 1] = '\0';
        uart_puts(buf);
    }
    uart_puts("\r\n");
    // --- STATUS Stage ---
    uart_puts("   Performing STATUS stage...\r\n");
    HCCHAR(channel) = (0 << 31) | // MPS: 64
                      (0 << 29) | // EPTYP: Control 
                      (0 << 27) | // DAD: 0 (default address)
                      (0 << 26) | // MC: 0
                      (0 << 25) | // EP: 0
                      (1 << 24) | // LSDEV: Low-speed device (change if needed)
                      (0 << 22) | // DevAddr: 0
                      (0 << 21) | // OddFrame: 0
                      (0 << 20);  // ChDis: 0 (enable channel)
    HCTSIZ(channel) = (0 << 19) | // PID: OUT
                      (0 << 0);   // XferSize: 0 bytes
    HCDMA(channel) = 0; // No data
    HCCHAR(channel) |= (1 << 31); // Enable channel
    // Poll for completion
    while ((HCINT(channel) & (1 << 0)) == 0); // Wait for Transfer Complete
    hcint = HCINT(channel);
    HCINT(channel) = hcint; // Clear interrupt
    if (hcint & (1 << 3)) {
        uart_puts("   STATUS stage failed.\r\n");
        return;
    }
    uart_puts("   STATUS stage completed.\r\n");

    uart_puts("--- USB practice finished. Hanging. ---\\r\n");
}
