/* USB HID Mouse for STM32F103C8
 * Robin Isaksson 2017 */
#include <stm32f10x.h>
#include <stdint.h>
#include "lib/system_clock.h"
#include "lib/timer.h"
#include "lib/gpio.h"
#include "lib/USART.h"
#include "lib/i2c.h"
#include "lib/USB.h"
#include "lib/USB_conf.h"

/*********************
 ***** CONSTANTS *****
 *********************/

#define CPU_CLK 72000000U
#define USART_BAUD_RATE 115200U

/************************
***** BEGIN PROGRAM *****
*************************/

HID_report_t HID_report = {0};

int main(void) {
    Init_System_Clocks();                       //Initialize system clocks
    Init_GPIO();                                //Initialize IO pins
    Init_TIM2();                                //Initialize Timer 2
    NVIC_EnableIRQ(TIM2_IRQn);                  //Enable timer 2 interrupts
    Init_USART(CPU_CLK, USART_BAUD_RATE);       //Initialize USART
    USART_Clear_Screen();                       //Clear the hyperterminal
    USART_Send_Str("\n\rUSART initialized...");
    Init_USB();                                 //Initialize USB
    USART_Send_Str("\n\rUSB initialized...");

    while(USB_Is_Not_Ready()) {
        /* Wait until the device is ready to transmit HID reports */
    };

    while(1) {
       /* Change values to make the mouse jitter */
        if (HID_report.x > 1) {
            HID_report.x = -2;
        } else {
            HID_report.x = 2;
        }

        /* Update the HID report and transmit */
        USB_Update_HID_Report();

        /* Delay 500ms and toggle the on-board LED */
        TIM2_Delay_ms(500U);
        LED_Toggle(GPIOC_BASE, 13);
    }
}
