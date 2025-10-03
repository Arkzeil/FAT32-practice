#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usb.h"
#include "usbendpoint.h"

typedef struct {
    unsigned char m_Address;
    USBSpeed m_Speed;
    // Endpoint 0 is always control type, bidirectional
    USBEndpoint* m_ControlEndpoint;
    boolean m_SplitTransfer;
    unsigned char m_HubAddress;
    unsigned char m_HubPortNumber;
    USBDeviceDescriptor *m_DeviceDescriptor;
    USBConfigurationDescriptor *m_ConfigurationDescriptors;
    unsigned char m_NumConfigurations;
} USBDevice;

#endif // USB_DEVICE_H