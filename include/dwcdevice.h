#ifndef DWC_DEVICE_H
#define DWC_DEVICE_H

// Device specific functions for DWC (DesignWare Core) USB controller
#include "types.h"
#include "usbendpoint.h"
#include "usbrequest.h"
#include "dwcrootport.h"

#define DWC_MAX_CHANNELS 16

#define DWHCI_CORE_HW_CFG2_NUM_HOST_CHANNELS(reg)	((((reg) >> 14) & 0xF) + 1)

#define DWHCI_HOST_CHAN_CHARACTER(chan)	(USB_HOST_BASE + 0x100 + (chan)*0x20) //
    #define DWHCI_HOST_CHAN_CHARACTER_MAX_PKT_SIZ__MASK	0x7FF
	#define DWHCI_HOST_CHAN_CHARACTER_EP_NUMBER__SHIFT	11
	#define DWHCI_HOST_CHAN_CHARACTER_EP_NUMBER__MASK	(0xF << 11)
	#define DWHCI_HOST_CHAN_CHARACTER_EP_DIRECTION_IN	(1 << 15)
	#define DWHCI_HOST_CHAN_CHARACTER_LOW_SPEED_DEVICE	(1 << 17)
	#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE__SHIFT	18
	#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE__MASK		(3 << 18)
		#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE_CONTROL	0
		#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE_ISO		1
		#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE_BULK		2
		#define DWHCI_HOST_CHAN_CHARACTER_EP_TYPE_INTERRUPT	3
	#define DWHCI_HOST_CHAN_CHARACTER_MULTI_CNT__SHIFT	20
	#define DWHCI_HOST_CHAN_CHARACTER_MULTI_CNT__MASK	(3 << 20)
	#define DWHCI_HOST_CHAN_CHARACTER_DEVICE_ADDRESS__SHIFT	22
	#define DWHCI_HOST_CHAN_CHARACTER_DEVICE_ADDRESS__MASK	(0x7F << 22)
	#define DWHCI_HOST_CHAN_CHARACTER_PER_ODD_FRAME		(1 << 29)
    #define DWHCI_HOST_CHAN_CHARACTER_DISABLE		(1 << 30)
	#define DWHCI_HOST_CHAN_CHARACTER_ENABLE		(1 << 31)

#define DWHCI_HOST_CHAN_SPLIT_CTRL(chan) (USB_HOST_BASE + 0x104 + (chan)*0x20)

#define DWHCI_HOST_CHAN_XFER_SIZ(chan)	(USB_HOST_BASE + 0x110 + (chan)*0x20)
	#define DWHCI_HOST_CHAN_XFER_SIZ_BYTES__MASK		0x7FFFF
	#define DWHCI_HOST_CHAN_XFER_SIZ_PACKETS__SHIFT		19
	#define DWHCI_HOST_CHAN_XFER_SIZ_PACKETS__MASK		(0x3FF << 19)
	#define DWHCI_HOST_CHAN_XFER_SIZ_PACKETS(reg)		(((reg) >> 19) & 0x3FF)
	#define DWHCI_HOST_CHAN_XFER_SIZ_PID__SHIFT		29
	#define DWHCI_HOST_CHAN_XFER_SIZ_PID__MASK		(3 << 29)
	#define DWHCI_HOST_CHAN_XFER_SIZ_PID(reg)		(((reg) >> 29) & 3)
		#define DWHCI_HOST_CHAN_XFER_SIZ_PID_DATA0 	0
		#define DWHCI_HOST_CHAN_XFER_SIZ_PID_DATA1 	2
		#define DWHCI_HOST_CHAN_XFER_SIZ_PID_DATA2 	1
		#define DWHCI_HOST_CHAN_XFER_SIZ_PID_MDATA 	3	// non-control transfer
		#define DWHCI_HOST_CHAN_XFER_SIZ_PID_SETUP 	3
#define DWHCI_HOST_CHAN_DMA_ADDR(chan)	(USB_HOST_BASE + 0x114 + (chan)*0x20)
// gap
#define DWHCI_HOST_CHAN_DMA_BUF(chan)	(USB_HOST_BASE + 0x11C + (chan)*0x20)	// DDMA only

// struct DWCTransferStageData; // Forward declaration

typedef struct{
    unsigned    channel;
    USBRequest *request;
    boolean     isIn;
    boolean     isStatusStage;
    // there should be split transfer related fields here, but we don't support split transfer for now

    USBDevice   *device; // The device this transfer is for
    USBEndpoint *endpoint; // The endpoint this transfer is for
    USBSpeed     speed; // Device speed
    unsigned int maxPacketSize; // Max packet size for the endpoint
    
    unsigned int m_TransferSize; // Total size of the transfer
    unsigned m_packets; // Total number of packets
    unsigned int m_BytesPerTransaction; // Bytes per transaction (maxPacketSize for full/low speed, 2*maxPacketSize for high speed)
    unsigned m_PacketsPerTransaction; // Packets per transaction (1 for full/low speed, 1-3 for high speed)
    unsigned int m_BytesTransferred; // Bytes transferred so far

    unsigned m_State; // State of the transfer
    unsigned m_SubState; // Sub-state of the transfer
    unsigned int m_TransactionState; // State within a transaction (e.g., sending, waiting for ACK, etc.)

    unsigned int __attribute__((aligned(4))) m_DMAAlignmentBuffer;
    void *m_BufferPtr; // Pointer to the current position in the buffer

    // there should be a scheduler related field here, but we don't implement scheduler for now
} DWCTransferStageData;

typedef struct{
    unsigned m_Channels;
    volatile unsigned m_ChannelsAllocated;  // one bit per channel
    volatile boolean m_Waiting;
    DWCTransferStageData m_StageData[DWC_MAX_CHANNELS];
    DWCRootPort m_RootPort;
} DWCDevice;

boolean DeviceInitCore(DWCDevice *dev);
boolean DeviceInitHost(DWCDevice *dev);
boolean DeviceEnableRootPort(DWCDevice *dev);
boolean DeviceInitRootPort(DWCRootPort *port);
boolean DeviceReset(unsigned int timeout);
boolean DeviceWaitForBit(unsigned int *reg, unsigned int bit, unsigned int value, unsigned int timeout);
USBSpeed DeviceGetPortSpeed();

int DWCDeviceControlMessage(DWCDevice *dev, USBEndpoint *ep, unsigned char requestType, unsigned char request, unsigned short value, unsigned short index, void* data, unsigned short size);
boolean DWCDeviceSubmitBlockingRequest(DWCDevice *dev, USBRequest *req);
boolean DWCDeviceTransferStage(DWCDevice* dev, USBRequest* req, boolean isIn, boolean isStatusStage);
boolean DWCDeviceTransferStageAsync(DWCDevice* dev, USBRequest* req, boolean isIn, boolean isStatusStage);
void DWCDeviceStartTransaction(DWCDevice *dev, DWCTransferStageData* stage);
void DWCDeviceStartChannel(DWCDevice *dev, DWCTransferStageData* stage);
unsigned char DWCTransferStageGetPID(DWCTransferStageData* stage);
unsigned char DWCTransferStageGetEndpointType(DWCTransferStageData* stage);

void DWCDeviceCompleteCallback(USBRequest* req, void *context);
unsigned DWCAllocChannel(DWCDevice* dev);

void DWCTransferStageDataInitialize(DWCTransferStageData* stage, unsigned channel, USBRequest* req, boolean isIn, boolean isStatusStage);


#endif // DWC_DEVICE_H