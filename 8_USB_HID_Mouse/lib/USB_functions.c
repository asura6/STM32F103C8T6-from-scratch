/* Robin Isaksson 2017 */ 
#include <stm32f10x.h>
#include <stdint.h>
#include "./USB_def.h"
#include "./USB_conf.h"
#include "./USB.h"
#include "./USB_functions.h"
#include "./USART.h"

/************************************************/
/*********   PREPROCESSOR DEFINITIONS   *********/
/************************************************/

#define EP_STAT_MASK (EP_STAT_TX | EP_STAT_RX)
#define EP_DTOG_MASK (EP_DTOG_TX | EP_DTOG_RX)
#define EP_TOG_MASK (EP_STAT_MASK | EP_DTOG_MASK)

#define USB_STATUS_DEFAULT          0x00U
#define USB_STATUS_ADDRESS_READY    0x01U
#define USB_STATUS_HID_READY        0x02U

/**********************************************/
/*********   PREPROCESSOR FUNCTIONS   *********/
/**********************************************/

#define SETUP_Direction_IN() \
    (setup_packet.bmRequestType & 0x80)

/* Descriptor booleans */
#define SETUP_Is_Get_Descriptor() \
     (setup_packet.bRequest == 0x06)
// These are further checks to isolate which kind of descriptor is requested
#define Is_Device_Descriptor() \
     (setup_packet.wValue == 0x01)
#define Is_Configuration_Descriptor() \
     (setup_packet.wValue == 0x02)
#define Is_Interface_Descriptor() \
     (setup_packet.wValue == 0x04)
#define Is_Endpoint_Descriptor() \
     (setup_packet.wValue == 0x05)
#define Is_Device_Qualifier_Descriptor() \
     (setup_packet.wValue == 0x06)
#define Is_Speed_Configuration_Descriptor() \
     (setup_packet.wValue == 0x07)
#define Is_Interface_Power_Descriptor() \
     (setup_packet.wValue == 0x08)
#define Is_HID_Descriptor() \
    (setup_packet.wValue == 0x22)

//String descriptors
#define Is_String_Descriptor() \
    ((setup_packet.wValue & 0x0FU) == 0x03)
#define Is_String_Descriptor_Zero() \
    ((setup_packet.wValue >> 8U) == 0x00)
#define Is_String_Descriptor_Manufacturer() \
    ((setup_packet.wValue >> 8U) == 0x01)
#define Is_String_Descriptor_Product() \
    ((setup_packet.wValue >> 8U) == 0x02)
#define Is_String_Descriptor_Serial() \
    ((setup_packet.wValue >> 8U) == 0x03)

#define SETUP_Is_Set_Address() \
    (setup_packet.bmRequestType == 0x00 && setup_packet.bRequest == 0x05)
#define SETUP_Is_Set_Configuration() \
    (setup_packet.bmRequestType == 0x00U \
     && setup_packet.bRequest == 0x09)
#define SETUP_Is_Set_Idle() \
    (setup_packet.bmRequestType == 0x21U && setup_packet.bRequest == 0x0a)

#define SETUP_Is_Get_Report() \
    (setup_packet.bRequest == 0x01U)

/*******************************/
/*********   Globals   *********/
/*******************************/

USB_Setup_Packet_t setup_packet;
uint8_t USB_status = USB_STATUS_DEFAULT;
uint8_t addr = 0;
extern HID_report_t HID_report;

/********************************************/
/*********   FORWARD DECLARATIONS   *********/
/********************************************/

void USB_Parse_SETUP(void);
void USB_Load_Descriptor(uint8_t ep, uint16_t *desc, uint16_t length);
void USB_Load_String_Descriptor(const USB_string_descriptor_t *desc);
void USB_Handle_HID_Interrupt(void);

/******************************************************/
/*********   ENDPOINT REGISTER FUNCTIONS   ************/
/******************************************************/

/* Set EP number ep TYPE to type */
void USB_EP_Set_Type(uint8_t ep, uint16_t type) {
    USB_EP(ep) = (USB_EP(ep) & ~(EP_TOG_MASK | EP_TYPE)) | type;
}

/* Set EP number ep KIND bit to value */
void USB_EP_Set_Kind(uint8_t ep, uint8_t value) {
    USB_EP(ep) = (USB_EP(ep) & ~(EP_TOG_MASK | EP_KIND)) | (value << 8U); //At bit place 8
}

/* Set EP number ep STAT RX to rx */
void USB_EP_Set_STAT_RX(uint8_t ep, uint16_t rx) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) |
        ((USB_EP(ep) & EP_STAT_RX) ^ (rx << 12U));
}

/* Set EP number ep STAT TX to tx */
void USB_EP_Set_STAT_TX(uint8_t ep, uint16_t tx) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) |
        ((USB_EP(ep) & EP_STAT_TX) ^ (tx << 4U));
}

/* Set EP number ep DTOG_RX to value */
void USB_EP_Set_DTOG_RX(uint8_t ep, uint16_t value) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) |
        ((USB_EP(ep) & EP_DTOG_RX) ^ (value << 14U));
}

/* Set EP number ep DTOG_TX to value */
void USB_EP_Set_DTOG_TX(uint8_t ep, uint16_t value) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) |
        ((USB_EP(ep) & EP_DTOG_TX) ^ (value << 6U));
}

void USB_EP_Clear_CTR_RX(uint8_t ep) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) & ~EP_CTR_RX;
}

void USB_EP_Clear_CTR_TX(uint8_t ep) {
    USB_EP(ep) = (USB_EP(ep) & ~EP_TOG_MASK) & ~EP_CTR_TX;
}

uint16_t USB_EP_Get_RX_Count(uint8_t ep) {
    return (USB_BDT(ep)->COUNT_RX & USB_COUNT0_RX_COUNT0_RX);
}

void USB_EP_Set_TX_Count(uint8_t ep, uint16_t count) {
    USB_BDT(ep)->COUNT_TX = count & USB_COUNT0_TX_COUNT0_TX;
}

/* Set EP number ep endpoint address (EA) to address */
void USB_EP_Set_EA(uint8_t ep, uint16_t address) {
    /* Clear EA bits */
    USB_EP(ep) = USB_EP(ep) & ~(EP_TOG_MASK | EP_EA);
    /* Set EA bits */
    USB_EP(ep) = (USB_EP(ep) & ~(EP_TOG_MASK)) |
        address;
}


/*********************************************************/
/*********   ENDPOINT TRANSACTION FUNCTIONS   ************/
/*********************************************************/

/* Handle a SETUP packet */
void USB_Handle_SETUP(void) {
    /* Reject further transaction until decided */
    USB_EP_Set_STAT_TX(EP0, EP_NAK);
    USB_EP_Set_STAT_RX(EP0, EP_NAK);
    /* Set DTOG bits as required for the SETUP transaction */
    USB_EP_Set_DTOG_TX(EP0, 1U);
    USB_EP_Set_DTOG_RX(EP0, 0U);

    /* Loads received values into a setup struct */
    USB_Parse_SETUP();

    if (SETUP_Direction_IN()) {

        if (SETUP_Is_Get_Descriptor()) {

            if (Is_Device_Descriptor()) {
                /* Transmit the device descriptor */
                USB_Load_Descriptor(EP0,
                        (uint16_t *)&USB_device_descriptor,
                        USB_device_descriptor.bLength);
                USB_EP_Set_STAT_TX(EP0, EP_VALID);

            } else if (Is_Configuration_Descriptor()) {
                /* If requested length longer than the configuration descriptor
                 * length then send the combined descriptors in one go otherwise
                 * just send the configuration descriptor */

                if ((setup_packet.wLength >> 8) > USB_combined_descriptor.conf_desc.bLength) {
                    USB_Load_Descriptor(EP0,
                            (uint16_t *)&USB_combined_descriptor,
                            USB_combined_descriptor.conf_desc.wTotalLength0);
                } else {
                    USB_Load_Descriptor(EP0,
                            (uint16_t *)&USB_combined_descriptor,
                            USB_combined_descriptor.conf_desc.bLength);
                }
                USB_EP_Set_STAT_TX(EP0, EP_VALID);

            } else if (Is_HID_Descriptor()) {
                /* Send the HID Descriptor if requested */
                USB_Load_Descriptor(EP0,
                        (uint16_t *)&HID_report_desc,
                        50U);
                USB_EP_Set_STAT_TX(EP0, EP_VALID);

            } else if (Is_String_Descriptor()) {
                /* Send any of the string descriptors if requested */
                if (Is_String_Descriptor_Zero()) {
                    USB_Load_String_Descriptor(
                            &USB_string_descriptor_zero);

                } else if (Is_String_Descriptor_Manufacturer()) {
                    USB_Load_String_Descriptor(
                            &USB_string_descriptor_manufacturer);

                } else if (Is_String_Descriptor_Product()) {
                    USB_Load_String_Descriptor(
                            &USB_string_descriptor_product);

                } else if (Is_String_Descriptor_Serial()) {
                    USB_Load_String_Descriptor(
                            &USB_string_descriptor_serial);
                }
                USB_EP_Set_STAT_TX(EP0, EP_VALID);

            } else {
                /* If unrecognized descriptor request then stall. This for
                 * example can be a device qualifier descriptor which the host
                 * might ask for to see if the device supports high-speed USB */
                USB_EP_Set_STAT_TX(EP0, EP_STALL);
            }

        } else if (SETUP_Is_Get_Report()) {
            /* Send the HID report through the control endpoint instead of the
             * interrupt endpoint */

            //Reset report as this should not be the report transmit mechanism
            HID_report = (HID_report_t){0};

            USB_Load_Descriptor(EP0,
                    (uint16_t *)(&HID_report),
                    HID_REPORT_SIZE);
            USB_EP_Set_STAT_TX(EP0, EP_VALID);
        }

    } else {
        /* Direction OUT */

        if (SETUP_Is_Set_Address()) {
            /* Set address request. This request should be completed after the
             * status stage so the USB_status variable is used as a
             * state-variable to complete this task */

            USB_status = USB_STATUS_ADDRESS_READY;
            /* Load address into a global value, changing the address later */
            addr = (setup_packet.wValue >> 8U) & USB_DADDR_ADD;
            USB_EP_Set_TX_Count(EP0, 0U);
            USB_EP_Set_STAT_TX(EP0, EP_VALID);

        } else if (SETUP_Is_Set_Configuration()) {
            /* Set configuration request */
            USB_EP_Set_TX_Count(EP0, 0U);
            USB_EP_Set_STAT_TX(EP0, EP_VALID);

        } else if (SETUP_Is_Set_Idle()) {
            /* The host requests the device to only update the reports when any
             * changes happen. We now activate the interrupt endpoint */

            //Successful request
            USB_EP_Set_TX_Count(EP0, 0U);
            USB_EP_Set_STAT_TX(EP0, EP_VALID);

            //Set global status that the USB is ready to transmitt HID reports
            USB_status = USB_STATUS_HID_READY;

            /* All HID reports have the same size so we set this once here */
            USB_EP_Set_TX_Count(EP1, HID_REPORT_SIZE);
        }
    }
}

void USB_Handle_OUT(void) {
    /* OUT transaction. Should not occur in this program */
}

void USB_Handle_IN(void) {
    if (USB_status == USB_STATUS_ADDRESS_READY) {
        /* If address is ready to be changed then change it */

        USB_EP_Clear_CTR_TX(EP0);
        USB_status = USB_STATUS_DEFAULT; //Default USB status
        USB->DADDR = addr | USB_DADDR_EF;
        USB_EP_Set_STAT_TX(EP0, EP_VALID);
        USB_EP_Set_STAT_RX(EP0, EP_VALID);

    } else if ((USB->ISTR & USB_ISTR_EP_ID) == 0x01U) {
        /* Endpoint one completed a transaction */

        USB_EP_Clear_CTR_TX(EP1);
        USB_EP_Set_STAT_TX(EP1, EP_NAK);

    } else {
        /* Control endpoint completed an IN transaction */

        USB_EP_Clear_CTR_TX(EP0);
        USB_EP_Set_STAT_TX(EP0, EP_VALID);
        USB_EP_Set_STAT_RX(EP0, EP_VALID);
    }
}

/* This function goes through the data contained in the received SETUP packet
 * and assigns the values to the setup_packet struct */
void USB_Parse_SETUP(void) {
    uint16_t byte_cnt = USB_EP_Get_RX_Count(EP0); //Bytes received
    uint16_t word_cnt = (byte_cnt + 1U) >> 1U;
    uint32_t addr_offset = USB_BDT(EP0)->ADDR_RX << 1U;
    uint16_t *src = (uint16_t *)(PMA_BASE + addr_offset);
    uint8_t data[byte_cnt];
    uint16_t *dst = (uint16_t *)data;

    //Load the received data into a data array
    while (word_cnt) {
        *dst = *src;
        dst += 1U;
        src += 2U;
        word_cnt --;
    }

    //Assign the values from the data array
    setup_packet.bmRequestType = data[0];
    setup_packet.bRequest = data[1];
    setup_packet.wValue = (data[2]<<8) | data[3];
    setup_packet.wIndex = (data[4]<<8) | data[5];
    setup_packet.wLength = (data[6]<<8) | data[7];
}

/* Load the descriptor data into the transmission register */
void USB_Load_Descriptor(uint8_t ep, uint16_t *desc, uint16_t length) {
    uint16_t word_cnt = (length + 1U) >> 1U;
    uint32_t addr_offset = USB_BDT(ep)->ADDR_TX << 1U;
    uint16_t *dst = (uint16_t *)(PMA_BASE + addr_offset);

    USB_EP_Set_TX_Count(ep, length);

    while (word_cnt) {
        *dst = *desc;
        dst += 2U;
        desc += 1U;
        word_cnt--;
    }
}

/* Load the string descriptor data into the transmission register */
void USB_Load_String_Descriptor(const USB_string_descriptor_t *desc) {
    uint16_t word_cnt = ((desc->bLength + 1U) >> 1U) - 1;
    uint32_t addr_offset = USB_BDT(EP0)->ADDR_TX << 1U;
    uint16_t *dst = (uint16_t *)(PMA_BASE + addr_offset);
    uint16_t *str_src = (uint16_t *)((uint32_t)desc->bString);

    USB_EP_Set_TX_Count(EP0, desc->bLength);

    //Load the first two bytes
    *dst = desc->bLength | desc->bDescriptorType << 8U;
    dst += 2U;

    //Load the rest of the bytes
    while (word_cnt) {
        *dst = *str_src;
        str_src += 1;
        dst += 2U;
        word_cnt--;
    }
}

/***************************************************/
/*********   ENDPOINT DEBUG FUNCTIONS   ************/
/***************************************************/

void USB_EP_Debug_Print(uint8_t ep) {
    uint16_t reg = USB_EP(ep);
    USART_Send_Str("\n\rEP: ");
    USART_Send_Integer(ep);
    USART_Send_Str(" | DUMP: | ");
    for (uint8_t i = 16U; i != 0; i--) {
        if (reg & (1U << (i - 1U))) {
            USART_Send_Integer(1U);
        } else {
            USART_Send_Integer(0U);
        }
        USART_Send_Str(" | ");
    }
}

void USB_ISTR_Debug_Print(void) {
    uint16_t reg = USB->ISTR;
    USART_Send_Str("\n\rISTR: | DUMP: | ");
    for (uint8_t i = 16U; i != 0; i--) {
        if (reg & (1U << (i - 1U))) {
            USART_Send_Integer(1U);
        } else {
            USART_Send_Integer(0U);
        }
        USART_Send_Str(" | ");
    }
}

void USB_SETUP_Debug_Print(void) {
    USART_Send_Str("\n\rSETUP PACKET: | ");
    USART_Send_Str("bmRequestType: ");
    USART_Send_Integer(setup_packet.bmRequestType);
    USART_Send_Str(" | bRequest: ");
    USART_Send_Integer(setup_packet.bRequest);
    USART_Send_Str(" | wValue: ");
    USART_Send_Integer(setup_packet.wValue);
    USART_Send_Str(" | wIndex: ");
    USART_Send_Integer(setup_packet.wIndex);
    USART_Send_Str(" | wLength: ");
    USART_Send_Integer(setup_packet.wLength);
}
