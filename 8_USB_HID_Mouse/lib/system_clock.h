#ifndef STM_SYSTEM_CLOCK
#define STM_SYSTEM_CLOCK

void Init_System_Clocks(void);
void System_Clock_Init_HSE(void);
void System_Clock_Init_PLL(void);
void System_Clock_Init_RTC(void);
void System_Clock_Init_LSE(void);
void System_Clock_Init_USB(void);

#endif
