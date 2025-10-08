#ifndef DWC_DEVICE_H
#define DWC_DEVICE_H

// Device specific functions for DWC (DesignWare Core) USB controller
#include "types.h"
#include "usbendpoint.h"
#include "usbrequest.h"
#include "dwcrootport.h"

#define DWC_MAX_CHANNELS 16

#define DWHCI_CORE_HW_CFG2_NUM_HOST_CHANNELS(reg)	((((reg) >> 14) & 0xF) + 1)

typedef struct{
    unsigned m_Channels;
    volatile unsigned m_ChannelsAllocated;  // one bit per channel
    volatile boolean m_Waiting;
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
void DWCDeviceCompleteCallback(USBRequest* req, void *context);
unsigned DWCAllocChannel(DWCDevice* dev);


#endif // DWC_DEVICE_H