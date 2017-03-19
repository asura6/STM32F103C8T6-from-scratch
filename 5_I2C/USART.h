#ifndef STM_USART
#define STM_USART

void Init_USART(uint32_t sys_clk, uint32_t baud_rate);
void USART_Send_Str(char *str);
void USART_Send_Char(int data); 
uint32_t USART_Receive(void);
void USART_Clear_Screen();

#endif
