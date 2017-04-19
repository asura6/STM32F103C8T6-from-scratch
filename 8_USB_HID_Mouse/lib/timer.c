#include <stm32f10x.h>
#include <stdint.h>
#include "./timer.h"

uint32_t global_counter = 0;

void Init_TIM2(void) {
    /* Enable clock to the TIM2 module */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Set the prescaler and auto-reload value to cause the timer to trigger
     * once every ms. The larger auto-reload value we have the better resolution
     * the timer has*/
    TIM2->PSC = 1U;
    TIM2->ARR = 36000U;
    /* Enable update interrupt */
    TIM2->DIER = TIM_DIER_UIE;
}

void TIM2_Delay_ms(uint32_t ms) {
    global_counter = 0;
    /* Reset counter */
    TIM2->CNT = 0x00U;
    /* Enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN;

    while (global_counter < ms) {
        /* Wait until count is complete */
        __asm("WFI");
    }
    /* Disable TIM2 counter */
    TIM2->CR1 &= ~TIM_CR1_CEN;
}

void TIM2_IRQ(void) {
    /* Reset the interrupt flag and increment the global counter */
    TIM2->SR &= ~TIM_SR_UIF;
    global_counter++;
}

