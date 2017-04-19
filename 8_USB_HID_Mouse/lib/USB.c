#include <stm32f10x.h>
#include <stdint.h>
#include "./USB_def.h"
#include "./USB_conf.h"
#include "./USB.h"
#include "./USB_functions.h"
#include "./USART.h"

/*******************************/
/*********   GLOBALS   *********/
/*******************************/

extern HID_report_t HID_report; //Defined in main.c
extern uint8_t USB_status; //A state variable defined in USB_functions.c.

/********************************************/
/*********   FORWARD DECLARATIONS   *********/
/********************************************/

void Init_USB(void);
void USB_Reset(void);
void USB_Addr_Debug_Print(void);

/******************************************/
/*********   BEGIN FUNCTIONS   ************/
/******************************************/

/* Initialize the USB peripheral */
void Init_USB(void) {
    /* Enable necessary USB clocks */
    /* USB pins are located on port A. Enable the port A clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    /* Set USB prescaler to 1.5 (72 MHz / 3 * 2 = 48 MHz */
    RCC->CFGR &= ~RCC_CFGR_USBPRE;
    /* Enable USB clock */
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    /* Configure pins as push-pull outputs */
    GPIOA->CRH |= GPIO_CRH_MODE11;   //Output mode, max 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF11;   //Push-pull
    GPIOA->CRH |= GPIO_CRH_MODE12;   //Output mode, max 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF12;   //Push-pull

    /* Pull Data lines low */
    GPIOA->BRR |= (1U << 12U) | (1U << 12U);

    for (uint16_t i = 0x100; i > 0; i--) {
        /* Minimal delay to allow unconnected appearance */
    }

    /* Setup the USB pins */
    /* PA11 is USB Data Minus */
    GPIOA->CRH |= GPIO_CRH_CNF11_1;  //Alternate function push-pull
    /* PA12 is USB Data Plus */
    GPIOA->CRH |= GPIO_CRH_CNF12_1;  //Alternate function push-pull
    /* Set pins high */
    GPIOA->BSRR |= (1 << 11U) | (1 << 12U);

    USB->CNTR &= ~USB_CNTR_PDWN;     //Exit Power Down mode
    for (uint16_t i = 0x100; i > 0; i--) {
        /* Delay more than 1 us to allow startup time */
    }
    USB->CNTR &= ~USB_CNTR_FRES;     //Clear forced USB reset
    USB->ISTR = 0x00U;               //Clear any spurious interrupts

    /* Enable CTRM and RESET interrupts */
    USB->CNTR |= (USB_CNTR_CTRM | USB_CNTR_RESETM);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn); //Enable the USB NVIC interrupts
}

void USB_Reset(void) {
    /* Enable device (and set the device address to zero */
    USB->DADDR = USB_DADDR_EF;
    /* Enable endpoints  */
    //Endpoint zero (control)
    USB_BDT(EP0)->ADDR_TX = 0x0040;
    USB_BDT(EP0)->ADDR_RX = 0x0080;
    //Endpoint one
    USB_BDT(EP1)->ADDR_TX = 0x00C0;
    /* Set block size to 32 bytes and number of blocks to 2 */
    USB_BDT(EP0)->COUNT_RX = USB_COUNT0_RX_BLSIZE | USB_COUNT0_RX_NUM_BLOCK_1;
    /* Set Buffer Description Table address */
    USB->BTABLE = 0x0000U;
    /* Set endpoint types */
    USB_EP_Set_Type(EP0, EP_CONTROL);
    USB_EP_Set_Type(EP1, EP_INTERRUPT);
    /* Set endpoint status */
    //EP0
    USB_EP_Set_STAT_TX(EP0, EP_VALID);
    USB_EP_Set_STAT_RX(EP0, EP_VALID);
    //EP1
    USB_EP_Set_EA(EP1, 0x01U); //Address
    USB_EP_Set_STAT_TX(EP1, EP_NAK); //EP1 should not transmit anything yet
}

/* Disconnects the USB device */
void USB_Disconnect(void) {
    USB->CNTR = USB_CNTR_FRES | USB_CNTR_PDWN;
    USB->ISTR = 0x00U;
}


/* USB Low-Priority Interrupt Handler */
void USB_LP_CAN1_RX0_IRQ(void) {
    uint16_t ISTR = USB->ISTR;

    if (ISTR & USB_ISTR_RESET) {
        /* RESET */
        USB->ISTR = ~USB_ISTR_RESET; //Clear interrupt
        USB_Reset();

    } else if (ISTR & USB_ISTR_CTR) {
        /* CTR */
        USB->ISTR = (uint16_t)~USB_ISTR_CTR; //Clear interrupt

        if (ISTR & USB_ISTR_DIR) {
            /* CTR_RX bit is set */
            USB_EP_Clear_CTR_RX(EP0); //Clear endpoint RX flag

            if (USB_EP(0) & EP_SETUP) {
                /* SETUP TRANSACTION */
                USB_Handle_SETUP();

            } else {
                /* OUT TRANSACTION */
                USB_Handle_OUT();
            }

        } else {
            /* CTR_TX should be set */
            USB_Handle_IN();
        }
    }
}

/* Loads the HID report into the transmission register and enables a
 * transmission. This is only done if the USB has had time to initialize */
void USB_Update_HID_Report(void) {
    USB_Load_Descriptor(EP1,
            (uint16_t *)(&HID_report),
            HID_REPORT_SIZE);
    USB_EP_Set_STAT_TX(EP1, EP_VALID);
}

/* When the device has received a SET IDLE command it is ready to transmit data
 * from the interrupt endpoint and this function starts returning zero-value */
uint8_t USB_Is_Not_Ready(void) {
    if (USB_status == 0x02U) {
        return 0;
    }
    return 1;
}

/* Debug USART print */
void USB_Addr_Debug_Print(void) {
    USART_Send_Str("\n\rAddress is: ");
    USART_Send_Integer(USB->DADDR & USB_DADDR_EF);
}
