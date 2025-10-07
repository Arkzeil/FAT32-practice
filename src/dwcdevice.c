#include "dwcdevice.h"
#include "utils.h"
#include "usbendpoint.h"
#include "uart.h"

int DWCDeviceControlMessage(DWCDevice* dev, USBEndpoint *ep, unsigned char requestType, unsigned char request, unsigned short value, unsigned short index, void* data, unsigned short size) {
    if(dev == NULL){
        uart_puts("DWCDeviceControlMessage: Device is NULL.\r\n");
        return -1;
    }

    ARM_ISB(); // ensure all previous memory accesses are completed before continuing

    __attribute__((aligned(4))) SetupData setup;    // Ensure 4-byte alignment, DMA buffer
    setup.bmRequestType = requestType;
    setup.bRequest = request;
    setup.wValue = value;
    setup.wIndex = index;
    setup.wLength = size;

    USBRequest req;
    req.m_Endpoint = ep;
    req.m_Setup = &setup;
    req.m_Buffer = data;
    req.m_BufferLength = size;
    req.m_status = 0;
    req.m_ResultLength = 0;
    req.m_CompletionCallback = NULL;

    int retLen = -1;

    if(DWCDeviceSubmitBlockingRequest(dev, &req)) {
        retLen = req.m_ResultLength;
    } else {
        uart_puts("DWCDeviceControlMessage: Request submission failed.\r\n");
    }

    return retLen;
}

boolean DWCDeviceSubmitBlockingRequest(DWCDevice* dev, USBRequest* req) {
    if(dev == NULL) {
        uart_puts("DWCDeviceSubmitBlockingRequest: Device is NULL.\r\n");
        return false;
    }

    ARM_ISB(); // ensure all previous memory accesses are completed before continuing

    if(req == NULL) {
        uart_puts("DWCDeviceSubmitBlockingRequest: Request is NULL.\r\n");
        return false;
    }

    req->m_status = 0; // Assume success for now

    if(req->m_Endpoint->m_Type == USBEndpointTypeControl){
        SetupData *setup = req->m_Setup;
        if(setup == NULL) {
            uart_puts("DWCDeviceSubmitBlockingRequest: Setup data is NULL for control endpoint.\r\n");
            return false;
        }

        if(setup->bmRequestType & REQUEST_IN){
            if(req->m_BufferLength <= 0){
                uart_puts("DWCDeviceSubmitBlockingRequest: Invalid buffer length for IN control transfer.\r\n");
                return false;
            }
        }
        else{

        }
    }

    return true;
}