#ifndef MY_BARE_USB_H
#define MY_BARE_USB_H

#include "types.h"
/**
 * @brief Initializes the DWC2 USB controller from scratch and performs
 *        basic enumeration steps.
 * 
 * This function is intended for low-level practice, directly manipulating
 * hardware registers based on the datasheet.
 */
void practice_usb_from_scratch();

#endif // MY_BARE_USB_H
