#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usb.h"
// #include "usbendpoint.h"

#define USBDEV_MAX_FUNCTIONS	10

struct USBEndpoint; // Forward declaration
struct DWCDevice; // Forward declaration

typedef struct {
    struct DWCDevice* m_HostController; // Pointer to the host controller
    unsigned char m_Address;
    USBSpeed m_Speed;
    // Endpoint 0 is always control type, bidirectional
    struct USBEndpoint* m_ControlEndpoint;
    boolean m_SplitTransfer;
    unsigned char m_HubAddress;
    unsigned char m_HubPortNumber;
    USBDeviceDescriptor *m_DeviceDescriptor;
    USBConfigurationDescriptor *m_ConfigurationDescriptors;
    unsigned char m_NumConfigurations;
} USBDevice;

void USBDeviceConstruct(USBDevice* device, struct DWCDevice* host_controller, USBSpeed speed, boolean split_transfer, unsigned char hub_address, unsigned char hub_port_number);

boolean USBDeviceInitialize(USBDevice* device);

#endif // USB_DEVICE_H