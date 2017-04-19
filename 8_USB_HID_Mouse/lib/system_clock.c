#include <stm32f10x.h>
#include <stdint.h>
#include "./system_clock.h"

/* We initialize the system clocks. We'll use an 8 MHz external oscillator (HSE)
 * as an input to the PLL without any prescaler.
 *
 * We multiply the PLL with 9 to get 72 MHz, the PLLCLK.
 *
 * We use the PLLCLK as source for the SYSCLK.
 *
 * We have no prescaler for the Cortex System timer, a /2 divisor prescaler to
 * the APB1 peripheral clock, and no prescaler for the APB2 peripheral clock
 *
 * Optionally we can use a USB prescaler on the 72 MHz clock to get 48 MHz USB
 * clock. This is done with a 1.5 prescaler divisor.
 *
 * We also use a 32.760 kHz external oscillator (LSE) to give us a 32.768 kHz
 * RTC.
 */


void Init_System_Clocks(void) {

    /* Enable the HSE */
    System_Clock_Init_HSE();
    /* Enable the PLL */
    System_Clock_Init_PLL();
    ///* Enable the LSE */
    //System_Clock_Init_LSE();
    ///* Enable the RTC */
    //System_Clock_Init_RTC();

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

void System_Clock_Init_LSE(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;

    RCC->BDCR |= RCC_BDCR_LSEON;
    /* Wait for the LSE to stabilize */
    for (uint16_t i = 0; i<0xFFFF; i++) {
        if (RCC->BDCR & RCC_BDCR_LSERDY) {
            break;
        }
    }

}

/* Unfinished ? */
void System_Clock_Init_RTC(void) {
    /* The LSE pins are located on port C on pins PC14 and PC15. This means we
     * need to enable the port clock to port C */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    PWR->CR |= PWR_CR_DBP;


    /* Set the RTC clock source as the low speed external crystal oscillator */
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

    /* Enable the RTC clock */
    RCC->BDCR |= RCC_BDCR_RTCEN;
}
