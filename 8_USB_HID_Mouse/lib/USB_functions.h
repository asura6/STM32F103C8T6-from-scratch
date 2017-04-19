#ifndef STM_USB_FUNCTIONS
#define STM_USB_FUNCTIONS

void USB_EP_Set_Type(uint8_t ep, uint16_t type);
void USB_EP_Set_Kind(uint8_t ep, uint8_t value);
void USB_EP_Set_STAT_RX(uint8_t ep, uint16_t rx);
void USB_EP_Set_STAT_TX(uint8_t ep, uint16_t tx);
void USB_EP_Set_DTOG_RX(uint8_t ep, uint16_t value);
void USB_EP_Set_DTOG_TX(uint8_t ep, uint16_t value);
void USB_EP_Clear_CTR_RX(uint8_t ep);
void USB_EP_Clear_CTR_TX(uint8_t ep);
uint16_t USB_EP_Get_RX_Count(uint8_t ep);
void USB_EP_Set_TX_Count(uint8_t ep, uint16_t count);
void USB_EP_Set_EA(uint8_t ep, uint16_t address);
void USB_Handle_SETUP(void);
void USB_Handle_OUT(void);
void USB_Handle_IN(void);
void USB_EP_Debug_Print(uint8_t ep);
void USB_ISTR_Debug_Print(void);
void USB_SETUP_Debug_Print(void);
void USB_Load_Descriptor(uint8_t ep, uint16_t *desc, uint16_t length);

#endif
