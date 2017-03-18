#include <stm32f10x.h>
#include <stdint.h> 
#include "./system_clock.h"

void Init_System_Clocks(void) { 
    /* Enable the HSE */
    System_Clock_Init_HSE();
    /* Enable the PLL */
    System_Clock_Init_PLL(); 

    /* Set the APB1 bus prescaler to /2 as 36 MHz it its max documented speed */ 
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 
    /* Set the flash latency to two wait-states as required for 72 MHz SYSCLK */
    FLASH->ACR |= FLASH_ACR_LATENCY_1; 
    /* Set the system clock to PLL */
    RCC->CFGR |= RCC_CFGR_SW_PLL; 
}

void System_Clock_Init_HSE(void) {
    /* The HSE pins are located on port D on pins PD0 and PD1. This means we
     * need to enable the port clock to port D */
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN; 

    /* Turn on HSE */
    RCC->CR |= RCC_CR_HSEON;

    /* Wait for the HSE to stabilize */
    for (uint16_t i = 0; i<0xFFFF; i++) { 
        if (RCC->CR & RCC_CR_HSERDY) { 
            break;
        }
    }
}

void System_Clock_Init_PLL(void) {
    /* Set the PLL multiplier to 9 */
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    /* Set the PLL source to the HSE (8 MHz external crystal) */
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    /* Turn on PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait the PLL it to stabilize */
    for (uint16_t i = 0; i<0xFFFF; i++) {
        if (RCC->CR & RCC_CR_PLLRDY) {
            break;
        }
    } 

} 
