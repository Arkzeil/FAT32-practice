#ifndef DWC_ROOT_PORT_H
#define DWC_ROOT_PORT_H

struct DWCDevice; // Forward declaration
struct USBDevice; // Forward declaration

typedef struct{
    struct DWCDevice *m_host;
    struct USBDevice *m_device;
} DWCRootPort;

#endif // DWC_ROOT_PORT_H