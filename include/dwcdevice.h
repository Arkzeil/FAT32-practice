#ifndef DWC_DEVICE_H
#define DWC_DEVICE_H

// Device specific functions for DWC (DesignWare Core) USB controller
#include "types.h"
#include "usbendpoint.h"
#include "usbrequest.h"

typedef struct{
    unsigned m_Channels;
    volatile unsigned m_ChannelsAllocated;  // one bit per channel
    volatile boolean m_Waiting;
    volatile boolean m_Active;
} DWCDevice;

int DWCDeviceControlMessage(DWCDevice* dev, USBEndpoint *ep, unsigned char requestType, unsigned char request, unsigned short value, unsigned short index, void* data, unsigned short size);
boolean DWCDeviceSubmitBlockingRequest(DWCDevice* dev, USBRequest* req);

#endif // DWC_DEVICE_H