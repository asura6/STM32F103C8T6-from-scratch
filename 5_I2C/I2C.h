#ifndef STM_I2C_
#define STM_I2C_

void Init_I2C(void); 
void I2C_Send_Byte(uint8_t address, uint8_t byte);
void I2C_Send_Bytes(uint8_t address, uint32_t nr_bytes, uint16_t *DMA_buffer); 
void I2C_Init_DMA();
void I2C_Wait_Until_Ready();

#endif
