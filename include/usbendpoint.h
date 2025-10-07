#ifndef USB_ENDPOINT_H
#define USB_ENDPOINT_H

#include "types.h"
#include "usbdevice.h"

typedef enum {
    USBEndpointTypeControl,
    USBEndpointTypeIsochronous,
    USBEndpointTypeBulk,
    USBEndpointTypeInterrupt
} USBEndpointType;

typedef struct{
    USBDevice* m_Device;
    unsigned char m_number;
    USBEndpointType m_Type;
    boolean m_DirectionIn; // true for IN, false for OUT
    unsigned int m_MaxPacketSize;
    unsigned m_Interval; // for interrupt and isochronous endpoints
    USBPID m_NextPID; // Data Toggle PID
} USBEndpoint;

void USBEndpointInit(USBEndpoint* ep, USBDevice* device);



#endif // USB_ENDPOINT_H