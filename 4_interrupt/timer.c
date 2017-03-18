#include <stm32f10x.h>
#include <stdint.h> 
#include "./timer.h"

void Init_TIM2(void) { 
    /* Enable clock to the TIM2 module */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 

    /* Set the prescaler to a high value */
    TIM2->PSC = 0xFFFFU; 
    /* Set the auto-reload value (max count number) */
    TIM2->ARR = 0x0225U; 
    /* Enable update interrupt */
    TIM2->DIER = TIM_DIER_UIE; 
    /* Enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN; 
}

uint8_t TIM2_Poll_Overflow(void) {
    /* If update interrupt flag return positive and reset flag */
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);
        return 1U;
    }
    /* Otherwise return zero */
    return 0U; 
}

uint16_t TIM2_Count(void) {
    return (uint16_t)TIM2->CNT;
}
