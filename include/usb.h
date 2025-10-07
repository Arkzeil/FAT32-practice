#ifndef USB_H
#define USB_H

#include "types.h"
// From BCM2835 15.USB section
#define USB_BASE        (MMIO_BASE + 0x980000)
#define USB_MDIO_CNTL   (USB_BASE + 0x080)      // MDIO interface control
#define USB_MDIO_GEN    (USB_BASE + 0x084)      // Data for MDIO interface  
#define USB_MDIO_DRV    (USB_BASE + 0x088)
#define USB_HOST_BASE   (USB_BASE + 0x400)      // Host mode register base
#define USB_POWER       (USB_BASE + 0xE00)      // Power and Clock Gating Control
// From https://github.com/rsta2/uspi/blob/master/include/uspi/dwhci.h#L97
// The DWC2 USB controller registers, we need to go to synopsys documentation for details
// (which need a registration to access)
#define DWHCI_CORE_RESET                        (USB_BASE + 0x010)  // Core Soft Reset register
    #define DWHCI_CORE_RESET_RX_FIFO_FLUSH		(1 << 4)
	#define DWHCI_CORE_RESET_TX_FIFO_FLUSH		(1 << 5)
	#define DWHCI_CORE_RESET_TX_FIFO_NUM__SHIFT	6
	#define DWHCI_CORE_RESET_TX_FIFO_NUM__MASK	(0x1F << 6)
#define DWHCI_CORE_VENDOR_ID                    (USB_BASE + 0x040)  // Vendor ID register
#define DWHCI_CORE_USB_CFG                      (USB_BASE + 0x00C)  // USB Configuration register
#define DWHCI_CORE_USB_CFG_PHYIF		        (1 << 3)
#define DWHCI_CORE_USB_CFG_ULPI_UTMI_SEL        (1 << 4)
#define DWHCI_CORE_USB_CFG_ULPI_FSLS		    (1 << 17)           // ULPI FS/LS select
#define DWHCI_CORE_USB_CFG_ULPI_CLK_SUS_M	    (1 << 19)           // ULPI clock suspendMode
#define DWHCI_CORE_USB_CFG_ULPI_EXT_VBUS_DRV	(1 << 20)           // ULPI external VBUS drive
#define DWHCI_CORE_USB_CFG_TERM_SEL_DL_PULSE	(1 << 22)           // Termination select for DL pulse 


#define DWHCI_CORE_RESET_AHB_IDLE		        (1 << 31)           // AHB Master IDLE

#define DWHCI_CORE_HW_CFG2		                (USB_BASE + 0x048)	// RO
#define DWHCI_CORE_HW_CFG2_OP_MODE(reg)			(((reg) >> 0) & 7)
#define DWHCI_CORE_HW_CFG2_ARCHITECTURE(reg)	(((reg) >> 3) & 3)
#define DWHCI_CORE_HW_CFG2_HS_PHY_TYPE(reg)		(((reg) >> 6) & 3)
#define DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_NOT_SUPPORTED		0
#define DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI			        1
#define DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_ULPI			        2
#define DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI_ULPI		    3
#define DWHCI_CORE_HW_CFG2_FS_PHY_TYPE(reg)		(((reg) >> 8) & 3)
#define DWHCI_CORE_HW_CFG2_FS_PHY_TYPE_DEDICATED		    1

#define DWHCI_CORE_USB_CFG_SRP_CAPABLE 		    (1 << 8)
#define DWHCI_CORE_USB_CFG_HNP_CAPABLE 		    (1 << 9)

#define DWHCI_CORE_AHB_CFG		                (USB_BASE + 0x008)
#define DWHCI_CORE_AHB_CFG_GLOBALINT_MASK	    (1 << 0)
#define DWHCI_CORE_AHB_CFG_MAX_AXI_BURST__SHIFT	1		// BCM2835 only
#define DWHCI_CORE_AHB_CFG_MAX_AXI_BURST__MASK	(3 << 1)	// BCM2835 only
#define DWHCI_CORE_AHB_CFG_WAIT_AXI_WRITES	    (1 << 4)	// BCM2835 only
#define DWHCI_CORE_AHB_CFG_DMAENABLE 		    (1 << 5)    // Enable DMA
#define DWHCI_CORE_AHB_CFG_AHB_SINGLE 		    (1 << 23)   // Enable AHB single transfer modes


#define DWHCI_HOST_CFG			                (USB_HOST_BASE + 0x000)
#define DWHCI_HOST_CFG_FSLS_PCLK_SEL__SHIFT	    0
#define DWHCI_HOST_CFG_FSLS_PCLK_SEL__MASK	    (3 << 0)
    #define DWHCI_HOST_CFG_FSLS_PCLK_SEL_30_60_MHZ	0
    #define DWHCI_HOST_CFG_FSLS_PCLK_SEL_48_MHZ	1
    #define DWHCI_HOST_CFG_FSLS_PCLK_SEL_6_MHZ	2
#define DWHCI_HOST_PORT 		                (USB_HOST_BASE + 0x040)
	#define DWHCI_HOST_PORT_CONNECT				(1 << 0)
	#define DWHCI_HOST_PORT_CONNECT_CHANGED		(1 << 1)
	#define DWHCI_HOST_PORT_ENABLE				(1 << 2)
	#define DWHCI_HOST_PORT_ENABLE_CHANGED		(1 << 3)
	#define DWHCI_HOST_PORT_OVERCURRENT			(1 << 4)
	#define DWHCI_HOST_PORT_OVERCURRENT_CHANGED	(1 << 5)
	#define DWHCI_HOST_PORT_RESET				(1 << 8)
	#define DWHCI_HOST_PORT_POWER				(1 << 12)
	#define DWHCI_HOST_PORT_SPEED(reg)			(((reg) >> 17) & 3)
		#define DWHCI_HOST_PORT_SPEED_HIGH		0
		#define DWHCI_HOST_PORT_SPEED_FULL		1
		#define DWHCI_HOST_PORT_SPEED_LOW		2
    #define DWHCI_HOST_PORT_DEFAULT_MASK		( DWHCI_HOST_PORT_CONNECT_CHANGED \
                                                | DWHCI_HOST_PORT_ENABLE	      \
                                                | DWHCI_HOST_PORT_ENABLE_CHANGED  \
                                                | DWHCI_HOST_PORT_OVERCURRENT_CHANGED)

#define USB_HOST_ID         0x0
#define USB_HCD_ID          0x1
#define USB_OTG_ID          0x2
#define USB_DEVICE_ID       0x3 

#define POWER_STATE_OFF      0x0
#define POWER_STATE_ON       0x1
#define POWER_STATE_WAIT     0x2

// Device Addresses
#define USB_DEFAULT_ADDRESS		    0   
#define USB_FIRST_DEDICATED_ADDRESS	1
#define USB_MAX_ADDRESS			    127

typedef enum{
    USBSpeedLow,
    USBSpeedFull,
    USBSpeedHigh,
    USBSpeedUnknown
} USBSpeed;

// This only contains few PIDs from token and data packets
// token packets: SETUP, IN, OUT
// data packets: DATA0, DATA1
// handshake packets: ACK, NAK, STALL, NYET
typedef enum{
    USBPIDSetup, 
    USBPIDData0,
    USBPIDData1,
} USBPID;

typedef struct{
    unsigned char  bmRequestType;
    unsigned char  bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} SetupData;

// Request Types
#define REQUEST_OUT			    0
#define REQUEST_IN			    0x80

#define REQUEST_CLASS			0x20
#define REQUEST_VENDOR			0x40

#define REQUEST_TO_INTERFACE	1
#define REQUEST_TO_OTHER		3

// Standard Request Codes
#define GET_STATUS			    0
#define CLEAR_FEATURE			1
#define SET_FEATURE			    3
#define SET_ADDRESS			    5
#define GET_DESCRIPTOR			6
#define SET_CONFIGURATION		9
#define SET_INTERFACE			11

// Descriptor Types
#define DESCRIPTOR_DEVICE		    1
#define DESCRIPTOR_CONFIGURATION	2
#define DESCRIPTOR_STRING		    3
#define DESCRIPTOR_INTERFACE		4
#define DESCRIPTOR_ENDPOINT		    5
#define DESCRIPTOR_CS_INTERFACE	    36
#define DESCRIPTOR_CS_ENDPOINT	    37

#define DESCRIPTOR_INDEX_DEFAULT	0

// see USB Mass Storage P.36
#define USB_DEFAULT_MAX_PACKET_SIZE 8
typedef struct{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned short bcdUSB;
    unsigned char  bDeviceClass;
    unsigned char  bDeviceSubClass;
    unsigned char  bDeviceProtocol;
    unsigned char  bMaxPacketSize0;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char  iManufacturer;
    unsigned char  iProduct;
    unsigned char  iSerialNumber;
    unsigned char  bNumConfigurations;
}__attribute__((packed)) USBDeviceDescriptor;

typedef struct{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned short wTotalLength;
    unsigned char  bNumInterfaces;
    unsigned char  bConfigurationValue;
    unsigned char  iConfiguration;
    unsigned char  bmAttributes;
    unsigned char  bMaxPower;
}__attribute__((packed)) USBConfigurationDescriptor;

typedef struct{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bInterfaceNumber;
    unsigned char  bAlternateSetting;
    unsigned char  bNumEndpoints;
    unsigned char  bInterfaceClass;
    unsigned char  bInterfaceSubClass;
    unsigned char  bInterfaceProtocol;
    unsigned char  iInterface;
}__attribute__((packed)) USBInterfaceDescriptor;

typedef struct{
    unsigned char  bLength;
    unsigned char  bDescriptorType;
    unsigned char  bEndpointAddress;
    unsigned char  bmAttributes;
    unsigned short wMaxPacketSize;
    unsigned char  bInterval;
}__attribute__((packed)) USBEndpointDescriptor;
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
boolean DeviceReset(unsigned int timeout);
boolean DeviceWaitForBit(unsigned int* reg, unsigned int bit, unsigned int value, unsigned int timeout);
USBSpeed DeviceGetPortSpeed();
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
