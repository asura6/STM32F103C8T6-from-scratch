#ifndef STM_USART
#define STM_USART

void Init_USART(uint32_t sys_clk, uint32_t baud_rate);
void USART_Send_Str(char *str); 
uint32_t USART_Receive(void);
void USART_Clear_Screen();
void USART_Send_Integer(uint32_t integer, char *format); 

#endif
