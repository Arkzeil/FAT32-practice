#include "usbendpoint.h"

void USBEndpointInit(USBEndpoint* ep, USBDevice* device) {
    if(ep == NULL || device == NULL)
        return;

    ep->m_Device = device;
    ep->m_number = 0; // Default endpoint number
    ep->m_Type = USBEndpointTypeControl; // Default to control type
    ep->m_DirectionIn = false; // Default to OUT direction
    ep->m_MaxPacketSize = USB_DEFAULT_MAX_PACKET_SIZE; // Default max packet size for full-speed control endpoint
    ep->m_Interval = 1; // Default interval for isochronous and interrupt endpoints
    ep->m_NextPID = USBPIDSetup; // Start with SETUP PID for control endpoint
}