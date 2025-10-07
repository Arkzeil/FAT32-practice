#ifndef USB_FUNCTION_H
#define USB_FUNCTION_H

#include "types.h"
#include "usbdevice.h"

typedef struct {
    unsigned char m_FunctionNumber;
    unsigned char m_InterfaceNumber;
    unsigned char m_AlternateSetting;
    unsigned char m_NumEndpoints;
    unsigned char m_InterfaceClass;
    unsigned char m_InterfaceSubClass;
    unsigned char m_InterfaceProtocol;
    unsigned char m_iInterface;
    USBDevice* m_Device;
} USBFunction;