#include "dwcdevice.h"
#include "utils.h"
#include "usbendpoint.h"
#include "usbdevice.h"
#include "uart.h"
#include "timer.h"
#include "lock.h"
#include "allocator.h"

boolean DeviceInitCore(DWCDevice *dev) {
    // uart_puts("TODO: Initialize USB device core...\r\n");
    unsigned int USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_ULPI_EXT_VBUS_DRV; // Clear ULPI external VBUS drive
    USBConfig &= ~DWHCI_CORE_USB_CFG_TERM_SEL_DL_PULSE;  // Set Termination select for DL pulse
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    DeviceReset(100); // 1 second timeout

    USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_ULPI_UTMI_SEL; // Select UTMI+PHY interface
    USBConfig &= ~DWHCI_CORE_USB_CFG_PHYIF; // Set PHY interface
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    unsigned int HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2);
    if(DWHCI_CORE_HW_CFG2_ARCHITECTURE(HWConfig2) != 2) { // Check architecture
        uart_puts("USB controller architecture is not supported!\r\n");
        return false;
    }
    HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2); // Read again
    // check PHY types
    if(DWHCI_CORE_HW_CFG2_HS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI_ULPI &&
        DWHCI_CORE_HW_CFG2_FS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_FS_PHY_TYPE_DEDICATED) 
    {
        HWConfig2 |= DWHCI_CORE_USB_CFG_ULPI_FSLS; // Set ULPI FS/LS select
        HWConfig2 |= DWHCI_CORE_USB_CFG_ULPI_CLK_SUS_M; // Set ULPI clock suspendMode
    }
    else{
        HWConfig2 &= ~DWHCI_CORE_USB_CFG_ULPI_FSLS; // Clear ULPI FS/LS select
        HWConfig2 &= ~DWHCI_CORE_USB_CFG_ULPI_CLK_SUS_M; // Clear ULPI clock suspendMode
    }
    mmio_write(DWHCI_CORE_USB_CFG, HWConfig2);

    if(dev->m_Channels != 0) {
        uart_puts("Device channels already initialized.\r\n");
        return false;
    }

    HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2);
    dev->m_Channels = DWHCI_CORE_HW_CFG2_NUM_HOST_CHANNELS(HWConfig2);
    ASSERT(4 <= dev->m_Channels && dev->m_Channels <= DWC_MAX_CHANNELS);

    unsigned int AHBConfig = mmio_read(DWHCI_CORE_AHB_CFG);
    AHBConfig |= DWHCI_CORE_AHB_CFG_DMAENABLE;            // Enable DMA
    AHBConfig |= DWHCI_CORE_AHB_CFG_WAIT_AXI_WRITES;      // Wait for AXI writes
    AHBConfig &= ~DWHCI_CORE_AHB_CFG_MAX_AXI_BURST__MASK; // Clear max AXI burst
    mmio_write(DWHCI_CORE_AHB_CFG, AHBConfig);

    USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    USBConfig &= ~DWHCI_CORE_USB_CFG_HNP_CAPABLE; // Clear HNP capable
    USBConfig &= ~DWHCI_CORE_USB_CFG_SRP_CAPABLE; // Clear SRP capable
    mmio_write(DWHCI_CORE_USB_CFG, USBConfig);

    return true;
}

boolean DeviceInitHost(DWCDevice *dev) {
    // uart_puts("TODO: Initialize USB host...\r\n");
    // restart PHY clock
    unsigned int Power = mmio_read(USB_POWER);
    Power = 0; // Set to 0 to restart PHY clock
    mmio_write(USB_POWER, Power);

    unsigned int HostConfig = mmio_read(DWHCI_HOST_CFG);
    HostConfig &= ~DWHCI_HOST_CFG_FSLS_PCLK_SEL__MASK; // Set FS/LS PCLK select to 0 (48MHz)

    unsigned int HWConfig2 = mmio_read(DWHCI_CORE_HW_CFG2);
    unsigned int USBConfig = mmio_read(DWHCI_CORE_USB_CFG);
    // set FS/LS PCLK select based on PHY types
    if(DWHCI_CORE_HW_CFG2_HS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_HS_PHY_TYPE_UTMI_ULPI &&
        DWHCI_CORE_HW_CFG2_FS_PHY_TYPE(HWConfig2) == DWHCI_CORE_HW_CFG2_FS_PHY_TYPE_DEDICATED &&
        (USBConfig & DWHCI_CORE_USB_CFG_ULPI_FSLS) )
    {
        HostConfig |= DWHCI_HOST_CFG_FSLS_PCLK_SEL_48_MHZ; // Set FS/LS PCLK select to 1 (48MHz)
    }
    else{
        HostConfig |= DWHCI_HOST_CFG_FSLS_PCLK_SEL_30_60_MHZ; // Set FS/LS PCLK select to 0 (30-60MHz)
    }
    mmio_write(DWHCI_HOST_CFG, HostConfig);

    // assume dynamic FIFO sizing is not supported, so we skip that part

    // flush all Tx FIFO
    unsigned int reset = mmio_read(DWHCI_CORE_RESET);
    reset |= DWHCI_CORE_RESET_TX_FIFO_FLUSH;
    reset &= ~DWHCI_CORE_RESET_TX_FIFO_NUM__MASK;
    reset |= (0x10 << DWHCI_CORE_RESET_TX_FIFO_NUM__SHIFT); // Flush
    mmio_write(DWHCI_CORE_RESET, reset);
    DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_TX_FIFO_FLUSH, 0, 1000000);

    // flush the Rx FIFO
    reset = mmio_read(DWHCI_CORE_RESET);
    reset |= DWHCI_CORE_RESET_RX_FIFO_FLUSH;
    mmio_write(DWHCI_CORE_RESET, reset);
    DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_RX_FIFO_FLUSH, 0, 1000000);

    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port control and status
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort = mmio_read(DWHCI_HOST_PORT); // Read again to clear
    if(!(HostPort & DWHCI_HOST_PORT_POWER)) {
        HostPort |= DWHCI_HOST_PORT_POWER; // Set port power
        mmio_write(DWHCI_HOST_PORT, HostPort);
    }

    return true;
}

boolean DeviceEnableRootPort(DWCDevice *dev) {
    // uart_puts("TODO: Enable USB root port...\r\n");
    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port control and status
    if(!DeviceWaitForBit((unsigned int*)DWHCI_HOST_PORT, DWHCI_HOST_PORT_CONNECT, 1, 1000000)) 
        return false;
    
    delay_ms(100); // USB 2.0 spec requires at least 100ms delay before enabling the port

    HostPort = mmio_read(DWHCI_HOST_PORT);      // Read again
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort |= DWHCI_HOST_PORT_ENABLE;         // Set port enable
    mmio_write(DWHCI_HOST_PORT, HostPort);

    delay_ms(50); // USB 2.0 spec

    HostPort = mmio_read(DWHCI_HOST_PORT);      // Read again
    HostPort &= ~DWHCI_HOST_PORT_DEFAULT_MASK; // Clear all status change bits
    HostPort &= ~DWHCI_HOST_PORT_RESET;         // Clear port reset
    mmio_write(DWHCI_HOST_PORT, HostPort);

    delay_ms(20);                               // Usually 10-20ms is enough

    return true;
}

boolean DeviceInitRootPort(DWCRootPort *port) {
    // uart_puts("TODO: Initialize USB root port...\r\n");
    USBSpeed speed = DeviceGetPortSpeed();
    if(speed == USBSpeedUnknown) {
        uart_puts("Unknown USB speed detected!\r\n");
        return false;
    }

    ASSERT(port->m_device == NULL);
    port->m_device = (USBDevice*)simple_malloc(sizeof(USBDevice));
    ASSERT(port->m_device != NULL);
    
    USBDeviceConstruct(port->m_device, port->m_host, speed, false, 0, 1); // Assume not a hub, so hub address is 0 and port number is 1

    if(!USBDeviceInitialize(port->m_device)) {
        uart_puts("Failed to initialize USB device on root port.\r\n");
        return false;
    }

    return true;
}

USBSpeed DeviceGetPortSpeed() {
    USBSpeed speed = USBSpeedUnknown;

    unsigned int HostPort = mmio_read(DWHCI_HOST_PORT); // Read host port

    switch(DWHCI_HOST_PORT_SPEED(HostPort)) {
        case DWHCI_HOST_PORT_SPEED_HIGH:
            speed = USBSpeedHigh;
            break;
        case DWHCI_HOST_PORT_SPEED_FULL:
            speed = USBSpeedFull;
            break;
        case DWHCI_HOST_PORT_SPEED_LOW:
            speed = USBSpeedLow;
            break;
        default:
            break;
    }

    return speed;
}

boolean DeviceReset(unsigned int timeout) {
    // uart_puts("TODO: Reset USB device...\r\n");

    // wait for AHB master IDLE
    if(!DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, DWHCI_CORE_RESET_AHB_IDLE, 1, timeout)) {
        uart_puts("Timeout waiting for AHB master IDLE\r\n");
        return false;
    }
    // set core soft reset
    mmio_write(DWHCI_CORE_RESET, 1);

    // wait for core soft reset to complete
    if(!DeviceWaitForBit((unsigned int*)DWHCI_CORE_RESET, 1, 0, timeout)) {
        uart_puts("Timeout waiting for core soft reset to complete\r\n");
        return false;
    }

    return true;
}

boolean DeviceWaitForBit(unsigned int* reg, unsigned int bit, unsigned int value, unsigned int timeout) {
    // need '?1:0' as directly take the result is not guaranteed to be 0 or 1
    while(((mmio_read((long)reg) & bit) ? 1 : 0) != value) {
        delay_ms(1);

        timeout--;
        if(timeout == 0) {
            uart_puts("Timeout waiting for bit\r\n");
            uart_puts("\r\n");
            return false;
        }
    }
    // if the value inside reg is unsigned int and its 31st bit is 1, it will be negative if directly cast to int
    // unsigned int reg_val = mmio_read((long)reg);
    // uart_itoa(reg_val);
    // uart_puts("\r\n");
    return true;
}

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

boolean DWCDeviceTransferStage(DWCDevice* dev, USBRequest* req, boolean isIn, boolean isStatusStage) {
    if(dev == NULL) {
        uart_puts("DWCDeviceTransferStage: Device is NULL.\r\n");
        return false;
    }

    if(req == NULL) {
        uart_puts("DWCDeviceTransferStage: Request is NULL.\r\n");
        return false;
    }

    req->m_CompletionCallback = &DWCDeviceCompleteCallback;

    if(dev->m_Waiting){
        uart_puts("DWCDeviceTransferStage: Device is already waiting on another request.\r\n");
        return false;
    }

    dev->m_Waiting = true;

    if(!DWCDeviceTransferStageAsync(dev, req, isIn, isStatusStage)) {
        uart_puts("DWCDeviceTransferStage: Async transfer initiation failed.\r\n");
        dev->m_Waiting = false;
        return false;
    }

    return req->m_status;
}   

boolean DWCDeviceTransferStageAsync(DWCDevice* dev, USBRequest* req, boolean isIn, boolean isStatusStage) {
    if(dev == NULL) {
        uart_puts("DWCDeviceTransferStageAsync: Device is NULL.\r\n");
        return false;
    }

    if(req == NULL) {
        uart_puts("DWCDeviceTransferStageAsync: Request is NULL.\r\n");
        return false;
    }

    unsigned channel = DWCAllocChannel(dev);
    if(channel >= dev->m_Channels) {
        uart_puts("DWCDeviceTransferStageAsync: No available channels.\r\n");
        return false;
    }



    return true;
}

void DWCDeviceCompleteCallback(USBRequest* req, void *context) {
    DWCDevice* dev = (DWCDevice*)context;
    if(dev == NULL) {
        uart_puts("DWCDeviceCompleteCallback: Context is NULL.\r\n");
        return;
    }

    dev->m_Waiting = false;

    // Additional context handling can be added here
}

unsigned DWCAllocChannel(DWCDevice* dev) {
    if(dev == NULL) {
        uart_puts("DWCAllocChannel: Device is NULL.\r\n");
        return -1;
    }
    // although we are in a single-threaded environment, we still use lock to protect critical section
    // just in case we want to port this code to a multi-threaded environment in the future
    lock();

    for(unsigned i = 0; i < dev->m_Channels; i++) {
        unsigned mask = 1 << i;
        if((dev->m_ChannelsAllocated & mask) == 0) {
            dev->m_ChannelsAllocated |= mask;
            unlock();
            return i;
        }
    }

    unlock();
    uart_puts("DWCAllocChannel: No available channels.\r\n");
    
    return DWC_MAX_CHANNELS; // No available channels
}