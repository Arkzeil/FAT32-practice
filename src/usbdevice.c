#include "usbdevice.h"
#include "usbendpoint.h"
#include "dwcdevice.h"
#include "allocator.h"
#include "uart.h"

void USBDeviceConstruct(USBDevice* device, struct DWCDevice* host_controller, USBSpeed speed, boolean split_transfer, unsigned char hub_address, unsigned char hub_port_number) {
    device->m_HostController = host_controller;
    device->m_Address = USB_DEFAULT_ADDRESS; // Default address
    device->m_Speed = speed;
    device->m_ControlEndpoint = 0;
    device->m_SplitTransfer = split_transfer;
    device->m_HubAddress = hub_address;
    device->m_HubPortNumber = 0;
    device->m_DeviceDescriptor = NULL; // To be filled after enumeration
    device->m_ConfigurationDescriptors = NULL; // To be filled after enumeration
    device->m_NumConfigurations = 0;

    device->m_ControlEndpoint = (USBEndpoint*)simple_malloc(sizeof(USBEndpoint));
    if(device->m_ControlEndpoint == NULL) {
        uart_puts("Control endpoint allocated failed.\r\n");
        return;
    }

    USBEndpointInit(device->m_ControlEndpoint, device);

    if(hub_port_number == 0) {
        uart_puts("Hub port number cannot be zero.\r\n");
        return;
    }

    // initialize Manufacturer and product string descriptors, skip for now

    for(unsigned fun = 0; fun < USBDEV_MAX_FUNCTIONS; fun++) {
        // initialize each function, skip for now
    }
}

boolean USBDeviceInitialize(USBDevice* device) {
    if(device == NULL) {
        uart_puts("Device is NULL.\r\n");
        return false;
    }

    if(device->m_DeviceDescriptor != NULL) {
        uart_puts("Device already initialized.\r\n");
        return false;
    }

    device->m_DeviceDescriptor = (USBDeviceDescriptor*)simple_malloc(sizeof(USBDeviceDescriptor));
    
    if(device->m_DeviceDescriptor == NULL) {
        uart_puts("Device descriptor allocation failed.\r\n");
        return false;
    }

    if(device->m_ControlEndpoint == NULL) {
        uart_puts("Control endpoint is NULL.\r\n");
        return false;
    }

    if(sizeof(device->m_DeviceDescriptor) > USB_DEFAULT_MAX_PACKET_SIZE) {
        uart_puts("Device descriptor size exceeds max packet size.\r\n");
        return false;
    }

    // Additional initialization steps can be added here

    return true;
}