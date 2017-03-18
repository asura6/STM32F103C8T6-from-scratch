#include <stm32f10x.h>
#include <stdint.h> 
#include "./timer.h"

void Init_TIM2(void) { 
    /* Enable clock to the TIM2 module */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 

    /* Here we set up how often the timer should overflow, that is which
     * frequency do we want to trigger of the timer.  We have a 72 MHz clock.
     * We use a prescaler which mean that the timer only increments 
     * 72MHz/(prescaler+1) times per second. We then use the auto-reload value
     * to say which value it should overflow at. Which mean that it overflows
     * 72MHz/(prescaler+1)/auto-reload times per second. With the values below
     * this evaluates to twice per second */ 
    /* Set prescaler */
    TIM2->PSC = 0xFFFFU; 
    /* Set the auto-reload value (max count number) */
    TIM2->ARR = 0x0225U; 
    /* Enable TIM2 counter */
    TIM2->CR1 |= TIM_CR1_CEN; 
} 
