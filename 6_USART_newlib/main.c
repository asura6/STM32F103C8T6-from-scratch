/* LED blinker for STM32F103C8
 * Robin Isaksson 2017
 */
#include <stm32f10x.h> 
#include <stdint.h>
#include "lib/system_clock.h"
#include "lib/timer.h"
#include "lib/gpio.h"
#include "lib/USART.h" 
#include "lib/i2c.h" 

#include <stdio.h>

/*********************
 ***** CONSTANTS *****
 *********************/

#define CPU_CLK 72000000U
#define USART_BAUD_RATE 38400U

/************************
***** BEGIN PROGRAM *****
*************************/

int main(void) { 
    Init_System_Clocks();                       //Initialize system clocks
    Init_GPIO();                                //Initialize IO pins 
    Init_TIM2();                                //Initialize Timer 2 
    NVIC_EnableIRQ(TIM2_IRQn);                  //Enable timer 2 interrupts
    Init_USART(CPU_CLK, USART_BAUD_RATE);       //Initialize USART 
    USART_Clear_Screen();                       //Clear the hyperterminal
    USART_Send_Str("\n\rUSART initialized...");
    Init_I2C();                                 //Initialize I2C
    NVIC_EnableIRQ(I2C1_EV_IRQn);               //Enable I2C event interrupts 
    USART_Send_Str("\n\rI2C initialized..."); 
    I2C_Init_DMA();                             //Initialize the I2C-DMA
    USART_Send_Str("\n\rDMA initialized..."); 

    USART_Send_Integer(0xFFFFFFFFU, "%u"); 

    /* Initialize I2C transfer address and some buffers containging data */ 
    uint8_t I2C_address = 0x27U;
    uint16_t DMA_buffer2[3] = {0xFFU, 0xFFU, 0xFFU};
    uint16_t DMA_buffer3[100]; 
    for (uint8_t i = 0; i <= 100U; i++) {
        DMA_buffer3[i] = 0xFFU;
    } 

    /* Send som bytes through I2C */
    I2C_Send_Bytes(I2C_address, 3U, DMA_buffer2); 
    I2C_Send_Byte(I2C_address, 0x0EEU); 
    I2C_Send_Bytes(I2C_address, 3U, DMA_buffer2); 
    I2C_Send_Bytes(I2C_address, 100U, DMA_buffer3); 

    /* Do nothing but wait for interrupts */
    while(1) { 
        TIM2_Delay_ms(500U);
        LED_Toggle(GPIOC_BASE, 13); 
    } 
} 
