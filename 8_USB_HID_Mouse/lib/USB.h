#ifndef STM_USB
#define STM_USB

void Init_USB(void);
void USB_Disconnect(void);
void USB_HID_Changed(void);
void USB_Update_HID_Report(void);
uint8_t USB_Is_Not_Ready(void);

#endif
