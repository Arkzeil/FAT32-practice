#ifndef USB_REQUEST_H
#define USB_REQUEST_H

#include "types.h"
#include "usb.h"
#include "usbendpoint.h"

struct USBRequest; // Forward declaration

typedef struct{
    USBEndpoint *m_Endpoint;
    SetupData *m_Setup; // Setup packet data
    void *m_Buffer; // Pointer to data buffer
    unsigned int m_BufferLength; // Length of data buffer
    int m_status; // Status of the request (e.g., success, error code)
    unsigned int m_ResultLength; // Actual length of data transferred
    // A completion callback
    void (*m_CompletionCallback)(struct USBRequest* req, void *context);
} USBRequest;

#endif // USB_REQUEST_H