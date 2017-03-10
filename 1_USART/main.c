/* LED blinker for STM32F103C8
 * Robin Isaksson 2017
 */
#include <stm32f10x.h> 
#include <stdint.h>

void Init_GPIO(void);
void init_USART(void);
void USART_Send_Str(char *str);
void USART_Send_Char(int data); 
uint32_t USART_Receive(void);
void LED_Flash(void);
void LED_Toggle(void); 

int main(void) {
    Init_GPIO();
    init_USART(); 

    USART_Send_Str("\n\rUSART COM Initiated\n\r"); 
    USART_Send_Str("This program converts upper case letters \
            to lower case letters\n\r"); 
    USART_Send_Str("Try it out and type something!\n\r\n\r"); 

    while(1) {
        /* Wait until something is received */
        uint32_t rec = USART_Receive();

        /* Flash LED to indicate activity */
        LED_Toggle();

        /* If a return character is received then return back a new line */
        if(rec == '\r') {
            USART_Send_Str("\n\r");
        } else {
            if (rec >= 'A' && rec < 'Z' + 1) {
                /* Convert to lower case */
                rec += 'a' - 'A';
            } else if (rec >= 'a' && rec < 'z' + 1) {
                /* Convert to upper case */
                rec += 'A' - 'a';
            } 
            /* Transmit character */ 
            USART_Send_Char(rec);

        } 
    } 
} 

void Init_GPIO(void) {
    /* Enable clock to IO port C */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    /* Set pin C13 as an output */
    GPIOC->CRH &= ~GPIO_CRH_MODE13; //Clear bits
    GPIOC->CRH |= GPIO_CRH_MODE13;  //Output mode, max 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;  //GPIO output push-pull

}

void init_USART(void) { 
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

    /* Set the baudrate for USART1 
    * USART_BRR = bus_frequency / target_baud_rate
    * this example: 8000000/38400 = 208 (rounded to integer) */
    USART1->BRR = 208;

    /* Enable USART1 and its RX- and TX-component */
    USART1->CR1 |= USART_CR1_UE // Usart enable
        | USART_CR1_TE          // Transmitter enable
        | USART_CR1_RE;         // Receiver enable 
}

uint32_t USART_Receive(void) {
    /* Wait until the data is ready to be received. */
    while ((USART1->SR & USART_SR_RXNE) == 0) { 
        /* Do nothing */
    } 
    /* read a maximum of 9 bits of RX data */
    return USART1->DR & 0x1FF;
}

void USART_Send_Char(int data) {
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

void LED_Toggle(void) { 
    /* If LED output is HIGH then change it to LOW */
    if (GPIOC->ODR & (1U << 13U)) {
        GPIOC->BRR = (1U << 13U); //Reset pin to LOW
    }   /* Otherwise change it to HIGH */
    else { 
        GPIOC->BSRR = (1U << 13U); //Set pin HIGH       

    } 
}
