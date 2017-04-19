#include <stm32f10x.h>
#include <stdint.h>
#include "USART.h"
#include "i2c.h"
#include "timer.h"

/*******************************
 ***** PREPROCESSOR MACROS *****
 *******************************/

#define I2C_Stop() I2C1->CR1 |= I2C_CR1_STOP
#define I2C_Start() I2C1->CR1 |= I2C_CR1_START
#define I2C_TX_Data(data) (I2C1->DR = (0x00FFU & data))
#define I2C_RX_Data() I2C1->DR
#define I2C_TX_Data_Empty() (I2C1->SR1 & I2C_SR1_TXE)
#define I2C_RX_Data_Not_Empty() (I2C1->SR1 & I2C_SR1_RXNE)
#define I2C_RX_Stop() (I2C1->SR1 & I2C_SR1_STOPF)
#define I2C_Byte_Transferred() (I2C1->SR1 & I2C_SR1_BTF)
#define I2C_Address_Sent() (I2C1->SR1 & I2C_SR1_ADDR)
#define I2C_Start_Condition() (I2C1->SR1 & I2C_SR1_SB)
#define I2C_Is_Busy() ((I2C1->SR2 & I2C_SR2_BUSY) ? 1U : 0U)

/************************************************
 ***** STATIC FUNCTION FORWARD DECLARATIONS *****
 ************************************************/

static void I2C_Update_DMA(uint32_t *DMA_buffer, uint32_t nr_bytes);

/****************************
 ***** GLOBAL VARIABLES *****
 ****************************/

/* We store the address in this global variable so that it can be accessed in
 * the interrupt service routines */
uint8_t I2C_address;
/* We use this global byte variable to enable it to be used by the ISRs */
uint8_t I2C_byte;

/*********************
 ***** CONSTANTS *****
 *********************/

#define TX_LSB 0U; //LSB of address for a transmission
#define RX_LSB 1U; //LSB of address for receiving data

/*************************
 ***** I2C FUNCTIONS *****
 *************************/

/* Initialize the I2C module
 * I2C1 Pins: SCL on PB6, SDA on PB7 */
void Init_I2C(void) {
    /* Enable the system clock for the I2C1 module */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; //Clock enable I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Clock enable port B
    /* Enable the alternate function on the pins (I2C function) and set them to
     * open drain-output configuration */
    GPIOB->CRL |= GPIO_CRL_MODE6_1; //2 MHz output mode
    GPIOB->CRL |= GPIO_CRL_CNF6_1;  //Alternate function open-drain
    GPIOB->CRL |= GPIO_CRL_MODE7_1; //2 MHz output mode
    GPIOB->CRL |= GPIO_CRL_CNF7_1;  //Alternate function open-drain
    /* Enable event interrupt */
    I2C1->CR2 |= I2C_CR2_ITEVTEN;
    /* Set the peripheral clock frequency */
    I2C1->CR2 |= (I2C_CR2_FREQ & 0x24U); //36 MHz

    /* Here we set the frequency to 100 kHz */
    /* Thigh = Tlow = CCR * Peripheral clock period
     * We'd like a standard time as 100 kHz which has a half-period of 5 us:
     * CCR = 5e-6/(1/36e6) = 180 = 0x00B4 */
    I2C1->CCR |= (I2C_CCR_CCR & 0x00B4U);
    /* P.S. If we want 400 kHz at 36 MHz peripheral clock we can use CRR = 30,
     * using DUTY bit = 0. Because
     * Thigh = CRR * Tpclk -> 1/(CRR*Tpclk + CRR*2*Tpclk) = 400e3 Hz */

    /* Set the maximum rise/fall time */
    /* This is calculated from the maximum allowed 1000 ns divided by the
     * peripheral time period + 1. In this case
     * TRISE = 1000e-9/(1/36e6) + 1 = 37 = 0x0025 */
    I2C1->TRISE = 0x0025U;

    /* End of configuration */
    /* Enable the peripheral */
    I2C1->CR1 |= I2C_CR1_PE;
}

/* Initialize the DMA module */
void I2C_Init_DMA(void) {
    /* Enable clock to the DMA 1 module */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    /* Set the peripheral address register to the I2C1 DR register */
    DMA1_Channel6->CPAR = (uint32_t)(&(I2C1->DR));
    /* Configure the channel priority to very high */
    DMA1_Channel6->CCR |= DMA_CCR6_PL;
    /* Set data direction to read from memory */
    DMA1_Channel6->CCR |= DMA_CCR6_DIR;
    /* Enable memory-increment mode */
    DMA1_Channel6->CCR |= DMA_CCR6_MINC;
    /* Set peripheral size to half-word size */
    DMA1_Channel6->CCR |= DMA_CCR6_PSIZE_0;
    /* Set memory-size to half-word size */
    DMA1_Channel6->CCR |= DMA_CCR6_MSIZE_0;
    /* Enable Transfer error and transfer complete interrupts */
    DMA1_Channel6->CCR |= DMA_CCR6_TEIE | DMA_CCR6_TCIE;
    /* Enable DMA1 Channel 6 interrupts */
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

/* Update the DMA module with new memory buffer information and then enable the
 * DMA channel */
static void I2C_Update_DMA(uint32_t *DMA_buffer, uint32_t nr_bytes) {
    /* Set the memory address that the DMA uses to read data from */
    DMA1_Channel6->CMAR = (uint32_t)(DMA_buffer);
    /* Set the number of data transfers */
    DMA1_Channel6->CNDTR = nr_bytes;
    /* Enable the DMA channel */
    DMA1_Channel6->CCR |= DMA_CCR6_EN;
}

/* Send a single byte through I2C */
void I2C_Send_Byte(uint8_t address, uint8_t byte) {
    /* Wait until BUS is ready */
    I2C_Wait_Until_Ready();
    /* Update global variables as the IRQ functions use them */
    I2C_address = (address << 1U) | TX_LSB;
    I2C_byte = byte;
    /* Start the bus and transititon the I2C module to master mode */
    I2C_Start();
    /* From here on the IRQ will transmit the data */
}

/* Send multiple bytes through I2C */
void I2C_Send_Bytes(uint8_t address, uint32_t nr_bytes, uint16_t *DMA_buffer) {
    /* Wait until BUS is ready */
    I2C_Wait_Until_Ready();
    /* Update global variables as the IRQ functions use them */
    I2C_address = (address << 1U) | TX_LSB;
    /* Update the DMA module with a pointer to the data-buffer and the number of
     * bytes to transmit */
    I2C_Update_DMA((uint32_t *)DMA_buffer, nr_bytes);
    /* Start the bus and transition the I2C module to master mode */
    I2C_Start();
    /* From here on the IRQs will transmit the data */
}

/* Wait until the I2C bus is ready to accept new transmissions */
void I2C_Wait_Until_Ready(void) {
    while (I2C_Is_Busy()) {
        /* Do nothing */
    }
}

/* I2C interrupt service routine */
void I2C1_EV_IRQ(void) {
    for (uint16_t i = 0; i < 0x02FFU; i++) {
        /* A delay is required here */
        /* Remove this naive delay and replace it with one using a timer with
         * NVIC and characterize the delay time required */
    }

    /* Check if start condition has been set */
    if (I2C1->SR1 & I2C_SR1_SB) {
        /* Transmit the address */
        I2C_TX_Data(I2C_address);
        return;
    }

    /* Check if the address has been sent */
    if (I2C1->SR1 & I2C_SR1_ADDR) {
        /* Check if the LSB of the address indicate transmit mode */
        if (I2C1->SR2 & I2C_SR2_TRA) {
            /* Check if the DMA is activated */
            if (DMA1_Channel6->CCR & DMA_CCR6_EN) {
                /* Enable DMA interrupt request which */
                I2C1->CR2 |= I2C_CR2_DMAEN;
                return;
            } else {
                /* Otherwise send data without DMA */
                I2C_TX_Data(I2C_byte);
                return;
            }
        }
    }

    /* Check if the transfer is finished */
    if (I2C1->SR1 & I2C_SR1_BTF) {
        /* Stop the I2C bus and complete the transaction */
        I2C_Stop();
        return;
    }
}

/* DMA interrupt service routine */
void DMA1_Channel6_IRQ(void) {

    /* Check for transfer error */
    if (DMA1->ISR & DMA_ISR_TEIF6) {
        /* Clear the interrupt flag */
        DMA1->IFCR |= DMA_IFCR_CTEIF6;
        return;
    }
    /* Check if the transfer is completed */
    if (DMA1->ISR & DMA_ISR_TCIF6) {
        /* Clear the interrupt flag */
        DMA1->IFCR |= DMA_IFCR_CTCIF6;
        /* Disable I2C DMA requests */
        I2C1->CR2 &= ~I2C_CR2_DMAEN;
        /* Disable the DMA channel */
        DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
        return;
    }
    /* Check for any other interrupt */
    if (DMA1->ISR & DMA_ISR_TCIF6) {
        /* Clear the interrupt flag */
        DMA1->IFCR |= DMA_IFCR_CGIF6;
    }
}
