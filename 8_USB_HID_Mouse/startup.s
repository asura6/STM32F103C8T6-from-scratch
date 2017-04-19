/* Startup code for a STM32F103C8
 * Robin Isaksson 2017
 */ 
.syntax unified
.arch armv7-m 
.thumb 

/* Initial vector table */
        .section "vectors" 
    .long    _STACK_TOP             /* Top of Stack */
    .long    _reset_Handler        /* Reset Handler */
    .long    _NMI_Handler           /* NMI Handler */
    .long    _HardFault_Handler     /* Hard Fault Handler */
    .long    _MemManage_Handler    /* Reserved */
    .long    _BusFault_Handler     /* Reserved */
    .long    _UsageFault_Handler   /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    _SVCall_Handler       /* SVCall Handler */
    .long    _DebugMonitor_Handler /* Reserved */
    .long    0                     /* Reserved */
    .long    _PendSV_Handler        /* PendSV Handler */
    .long    _SysTick_Handler       /* SysTick Handler */
/* External interrupts */
    .long   WWDG_IRQ
    .long   PVD_IRQ
    .long   TAMPER_IRQ
    .long   RTC_IRQ
    .long   FLASH_IRQ
    .long   RCC_IRQ
    .long   EXTI0_IRQ
    .long   EXTI1_IRQ
    .long   EXTI2_IRQ
    .long   EXTI3_IRQ
    .long   EXTI4_IRQ
    .long   DMA1_Channel1_IRQ
    .long   DMA1_Channel2_IRQ
    .long   DMA1_Channel3_IRQ
    .long   DMA1_Channel4_IRQ
    .long   DMA1_Channel5_IRQ
    .long   DMA1_Channel6_IRQ
    .long   DMA1_Channel7_IRQ
    .long   ADC1_2_IRQ
    .long   USB_HP_CAN1_TX_IRQ
    .long   USB_LP_CAN1_RX0_IRQ
    .long   CAN1_RX1_IRQ
    .long   CAN1_SCE_IRQ
    .long   EXTI9_5_IRQ
    .long   TIM1_BRK_IRQ
    .long   TIM1_UP_IRQ
    .long   TIM1_TRG_COM_IRQ
    .long   TIM1_CC_IRQ
    .long   TIM2_IRQ
    .long   TIM3_IRQ
    .long   TIM4_IRQ
    .long   I2C1_EV_IRQ
    .long   I2C1_ER_IRQ
    .long   I2C2_EV_IRQ
    .long   I2C2_ER_IRQ
    .long   SPI1_IRQ
    .long   SPI2_IRQ
    .long   USART1_IRQ
    .long   USART2_IRQ
    .long   USART3_IRQ
    .long   EXTI15_10_IRQ
    .long   RTCAlarm_IRQ
    .long   USBWakeUp_IRQ

/* Startup code */
.text
.thumb_func
_reset_Handler: 

        /* Copy data from flash to RAM */
_startup: 
        @@ Copy data to RAM 
        ldr     r0, =FLASH_DATA_START
        ldr     r1, =RAM_DATA_START
        ldr     r2, =RAM_DATA_SIZE 
        @@ If data is zero then do not copy 
        cmp     r2, #0 
        beq     _BSS_init 
        /* Initialize data by loading it to RAM */
        ldr     r3, =0 @@ i = 0 
_RAM_copy: 
        ldr    r4, [r0, r3] @@ r4 = FLASH_DATA_START[i] 
        str    r4, [r1, r3] @@ RAM_DATA_START[i] = r4 
        adds    r3, #4       @@ i++ 
        cmp     r3, r2       @@ if i < RAM_DATA_SIZE then branch 
        blt     _RAM_copy    @@ otherwise loop again 

        /* Initialize uninitialized variables to zero (required for C) */
_BSS_init: 
        ldr     r0, =__bss_start__
        ldr     r1, =__bss_end__
        ldr     r2, =bss_size
        /* If BSS size is zero then do not initialize */
        cmp     r2, #0
        beq     _end_of_init
        /* Initialize BSS variables to zero */
        ldr     r3, =0x0 @@ i = 0 
        ldr     r4, =0x0 @@ r4 = 0 
_BSS_zero: 
        str     r4, [r0, r3] @@ BSS_START[i] = 0
        adds    r3, #4       @@ i++
        cmp     r3, r2       @@ if i < BSS_SIZE then branch
        blt     _BSS_zero        @@ otherwise loop again 

        /* Here control is given to the C-main function. Take note that
         * no heap is initialized. */
_end_of_init:   bl   main 
_halt:  b       _halt 

/* Here are all the interrupt vectors. The macro below define each and every
 * one of the ones listed beolw as a weak symbol which can be overwritten by
 * including the corresponding function-name in a C-file (for example) */
.macro                 def_rewritable_handler   handler 
    .thumb_func
    .weak    \handler
    .type    \handler, %function
    \handler:   b  . @@ Branch forever in default state
.endm
       
def_rewritable_handler  _NMI_Handler             /* NMI HANDLER */
def_rewritable_handler  _HardFault_Handler       /* HARD FAULT Handler */
def_rewritable_handler  _MemManage_Handler    /* Reserved */
def_rewritable_handler  _BusFault_Handler     /* Reserved */
def_rewritable_handler  _UsageFault_Handler   /* Reserved */ 
def_rewritable_handler  _SVCall_Handler             /* SVCALL Handler */
def_rewritable_handler  _DebugMonitor_Handler /* Reserved */ 
def_rewritable_handler  _PendSV_Handler          /* PENDSV Handler */
def_rewritable_handler  _SysTick_Handler         /* SYSTICK Handler */ 
def_rewritable_handler  WWDG_IRQ
def_rewritable_handler  PVD_IRQ
def_rewritable_handler  TAMPER_IRQ
def_rewritable_handler  RTC_IRQ
def_rewritable_handler  FLASH_IRQ
def_rewritable_handler  RCC_IRQ
def_rewritable_handler  EXTI0_IRQ
def_rewritable_handler  EXTI1_IRQ
def_rewritable_handler  EXTI2_IRQ
def_rewritable_handler  EXTI3_IRQ
def_rewritable_handler  EXTI4_IRQ
def_rewritable_handler  DMA1_Channel1_IRQ
def_rewritable_handler  DMA1_Channel2_IRQ
def_rewritable_handler  DMA1_Channel3_IRQ
def_rewritable_handler  DMA1_Channel4_IRQ
def_rewritable_handler  DMA1_Channel5_IRQ
def_rewritable_handler  DMA1_Channel6_IRQ
def_rewritable_handler  DMA1_Channel7_IRQ
def_rewritable_handler  ADC1_2_IRQ
def_rewritable_handler  USB_HP_CAN1_TX_IRQ
def_rewritable_handler  USB_LP_CAN1_RX0_IRQ
def_rewritable_handler  CAN1_RX1_IRQ
def_rewritable_handler  CAN1_SCE_IRQ
def_rewritable_handler  EXTI9_5_IRQ
def_rewritable_handler  TIM1_BRK_IRQ
def_rewritable_handler  TIM1_UP_IRQ
def_rewritable_handler  TIM1_TRG_COM_IRQ
def_rewritable_handler  TIM1_CC_IRQ
def_rewritable_handler  TIM2_IRQ
def_rewritable_handler  TIM3_IRQ
def_rewritable_handler  TIM4_IRQ
def_rewritable_handler  I2C1_EV_IRQ
def_rewritable_handler  I2C1_ER_IRQ
def_rewritable_handler  I2C2_EV_IRQ
def_rewritable_handler  I2C2_ER_IRQ
def_rewritable_handler  SPI1_IRQ
def_rewritable_handler  SPI2_IRQ
def_rewritable_handler  USART1_IRQ
def_rewritable_handler  USART2_IRQ
def_rewritable_handler  USART3_IRQ
def_rewritable_handler  EXTI15_10_IRQ
def_rewritable_handler  RTCAlarm_IRQ
def_rewritable_handler  USBWakeUp_IRQ

        .end 
