#include <stdint.h>
#include <stm32f10x.h>

void Init_GPIO(void) {
    /* Enable clock to IO port C */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    /* Set pin C13 as an output */
    GPIOC->CRH &= ~GPIO_CRH_MODE13; //Clear bits
    GPIOC->CRH |= GPIO_CRH_MODE13;  //Output mode, max 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;  //GPIO output push-pull
}

/**
 * @brief Switch state of a pin.
 *
 * @param port  The base-memory address to the port the pin is on. Example:
 * GPIOC_BASE would correspond to port c using the "stm32f10x.h" definitions.
 * @param pin   The pin-number.
 */
void LED_Toggle(uint32_t port, uint32_t pin) {
    /* We assign pointers to the memory-addresses for the registers we use to
     * toggle the LED. The offset values 0x0C, 0x10, 0x14 can be found in the
     * reference manual for the microcontroller */
    uint32_t *ODR = (uint32_t *)(port + 0x0C);
    uint32_t *BSRR = (uint32_t *)(port + 0x10);
    uint32_t *BRR = (uint32_t *)(port + 0x14);

    /* If output is HIGH then change it to LOW and vice versa */
    if ((*ODR) & (1U << pin)) {
        (*BRR) = (1U << pin); //Set pin LOW
    } else {
        (*BSRR) = (1U << pin); //Set pin HIGH
    }
}
