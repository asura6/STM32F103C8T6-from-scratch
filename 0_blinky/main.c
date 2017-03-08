/* LED blinker for STM32F103C8
 * Robin Isaksson 2017
 */
#include <stm32f10x.h> 
#include <stdint.h>

void delay(void);

int main(void) {
    /* Enable clock to IO port C */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    /* Set pin C13 as an output */
    GPIOC->CRH &= ~GPIO_CRH_MODE13; //Clear bits
    GPIOC->CRH |= GPIO_CRH_MODE13;  //Output mode, max 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;  //GPIO output push-pull

    while(1) {
        GPIOC->BSRR = (1U << 13U); //Set pin HIGH
        delay();
        GPIOC->BRR = (1U << 13U); //Reset pin to LOW
        delay();
    }
}

/* Primitive delay function */
void delay(void) {
    uint8_t i; 
    uint8_t j;

    for (i = 0; i < 0xFF; i++) {
        for (j = 0; j < 0xFF; j++) { 
        }
    } 
}
