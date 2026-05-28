#include "stm32l476xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void USART_Write(USART_TypeDef *USARTx, char *buffer, uint32_t nBytes);
void USART_Read(USART_TypeDef *USARTx, char *buffer, uint32_t nBytes);
void UART1_init(void);
void UART2_init(void);
void UART3_init(void);
