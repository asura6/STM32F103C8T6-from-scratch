#include <stm32f10x.h>
#include <stdint.h>
#include "USART.h"

void USART1_IRQ(void) {
    /* Clear interrupt flag by reading from DR register */
    (void)(USART1->DR);
    USART_Send_Str("\n\rUSART triggered an interrupt");
}

void Init_USART(uint32_t sys_clk, uint32_t baud_rate) {
    /* Enable the system clock for USART1 module */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //Clock enable usart1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Clock enable port A

    /* Setup the RX/TX pins for USART1 */
    GPIOA->CRH |= GPIO_CRH_MODE9;   //Output mode, max 50 MHz
    GPIOA->CRH &= ~GPIO_CRH_CNF9;   //Clear bits
    GPIOA->CRH |= GPIO_CRH_CNF9_1;  //Alternate function push-pull
    GPIOA->CRH &= ~GPIO_CRH_MODE10; //Input mode
    GPIOA->CRH &= ~GPIO_CRH_CNF10;  //Clear bits
    GPIOA->CRH |= GPIO_CRH_CNF10_0; //Floating input

    /* Set the baudrate for USART1 */
    USART1->BRR = sys_clk/baud_rate;

    /* Enable USART1 and its RX- and TX-component */
    USART1->CR1 |= USART_CR1_UE // Usart enable
        | USART_CR1_TE          // Transmitter enable
        | USART_CR1_RE          // Receiver enable
        | USART_CR1_RXNEIE;     // RXNE interrupt enable

    /* Enable USART interrupts */
    NVIC_EnableIRQ(USART1_IRQn);
}

uint32_t USART_Receive(void) {
    /* Wait until the data is ready to be received. */
    while ((USART1->SR & USART_SR_RXNE) == 0) {
        /* Do nothing */
    }
    /* read a maximum of 9 bits of RX data */
    return USART1->DR & 0x1FF;
}

static void USART_Send_Char(int data) {
    /* Load the data into the register to send it */
    USART1->DR = data;

    /* Wait until transmit completes */
    while ((USART1->SR & USART_SR_TXE) == 0) {
        /* Do nothing until transmitted */
    }
}

void USART_Send_Str(char *str) {
    uint8_t i = 0;
    /* Keep sending characters until null-byte is encountered */
    while(str[i] != 0) {
        USART_Send_Char(str[i++]);
    }
}

void USART_Clear_Screen(void) {
    USART_Send_Str("\033[2J");
}

void USART_Send_Integer(uint32_t value) {
    /* We test for the most probable cases */
    if (value == 0) {
        USART_Send_Char('0');
        return;
    } else if (value == 1) {
        USART_Send_Char('1');
        return;
    }

    char buffer[11]; //Buffer to store digits
    uint8_t i = 0;
    uint32_t exp = 1000000000U;
    /* This method iterates through the digits using integer division */
    while (value > 0) {
       buffer[i] = value / exp;
       value -= buffer[i] * exp;
       buffer[i] += '0'; //Convert to ASCII
       exp /= 10;
       i++;
    }
    buffer[i] = '\0'; //Null terminator

    /* Find number of leading zeros */
    for (i = 0; i <= 10; i++) {
        if (buffer[i] != '0') {
            break;
        }
    }
    /* Remove leading zeros */
    char out[11-i]; //Array to store the output string
    for (uint8_t j = 0; j <= 10-i; j++) {
        out[j] = buffer[j+i];
    }
    out[11-i] = '\0'; //Null terminator
    /* Print result */
    USART_Send_Str(out);
}
