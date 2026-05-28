#include "stm32l476xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"


void UART1_init() {
	//GPIOA UART1 INIT

	GPIOA->MODER &= 0xFF28FFFF;
	GPIOA->MODER |= 0x00280000;
	
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2;
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2;
	
	GPIOA->OSPEEDR |= 0x003C0000; // High speed PA9/PA10
	
	GPIOA->PUPDR |= 0x00280000; // Pull DOWN
	GPIOA->PUPDR &= 0xFF28FFFF;
	
	GPIOA->OTYPER &= 0xFFFFFFF3; // Push Pull
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL); // Sys clock
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;
	
	
	USART1->CR1 &= ~USART_CR1_UE;			//Disable uart
	USART1->CR1 &= ~USART_CR1_M;			//set data bits
	USART1->CR2 &= ~USART_CR2_STOP;		//set  STOP BITS
	USART1->CR1 &= ~USART_CR1_PCE;		//disable parity control
	USART1->CR1 &= ~USART_CR1_OVER8;	// set oversampling by 16
	USART1->BRR = 0x683;							//BAUD = 9600
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // Trans/Rec enable
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1  |= USART_CR1_RXNEIE;
	while((USART1->ISR & USART_ISR_TEACK) == 0); // Ready for transmission
	while((USART1->ISR & USART_ISR_REACK) == 0); // Ready for reception	
	
	NVIC_EnableIRQ(USART1_IRQn);
}

//void UART3_init() {
//	//GPIOA UART3 INIT

//	GPIOC->MODER &= 0xFF0FFFFF;
//	GPIOC->MODER |= 0x00A00000;
//	
//	GPIOC->AFR[1] |= 0x00001000;
//	GPIOC->AFR[1] |= 0x00001000;
//	
//	GPIOC->OSPEEDR |= 0x00A00000; // High speed PB10/PB11
//	
//	GPIOC->PUPDR &= 0xFF0FFFFF;
//	GPIOC->PUPDR |= 0x00A00000; // Pull DOWN
//	
//	//GPIOB->OTYPER |= 0xFFFFFCFF; // Push Pull
//	
//	
//	RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;
//	
//	RCC->CCIPR &= ~(RCC_CCIPR_UART4SEL); // Sys clock
//	RCC->CCIPR |= RCC_CCIPR_UART4SEL_0;
//	
//	
//	UART4->CR1 &= ~USART_CR1_UE;			//Disable uart
//	UART4->CR1 &= ~USART_CR1_M;			//set data bits
//	UART4->CR2 &= ~USART_CR2_STOP;		//set  STOP BITS
//	UART4->CR1 &= ~USART_CR1_PCE;		//disable parity control
//	UART4->CR1 &= ~USART_CR1_OVER8;	// set oversampling by 16
//	UART4->BRR = 0x683;							//BAUD = 9600
//	UART4->CR1 |= USART_CR1_TE | USART_CR1_RE; // Trans/Rec enable
//	UART4->CR1 |= USART_CR1_UE;
//	UART4->CR1  |= USART_CR1_RXNEIE;
//	while((UART4->ISR & USART_ISR_TEACK) == 0); // Ready for transmission
//	while((UART4->ISR & USART_ISR_REACK) == 0); // Ready for reception	
//	
//	NVIC_EnableIRQ(UART4_IRQn);
//}


// uart3 init on gpio b
void UART3_init() {
	//GPIOA UART3 INIT

	GPIOB->MODER &= 0xFF0FFFFF;
	GPIOB->MODER |= 0x00A00000;
	
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL11_0 | GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2;
	
	GPIOB->OSPEEDR |= 0x00A00000; // High speed PB10/PB11
	
	GPIOB->PUPDR &= 0xFF0FFFFF;
	GPIOB->PUPDR |= 0x00A00000; // Pull DOWN
	
	//GPIOB->OTYPER |= 0xFFFFFCFF; // Push Pull
	
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	
	RCC->CCIPR &= ~(RCC_CCIPR_USART3SEL); // Sys clock
	RCC->CCIPR |= RCC_CCIPR_USART3SEL_0;
	
	
	USART3->CR1 &= ~USART_CR1_UE;			//Disable uart
	USART3->CR1 &= ~USART_CR1_M;			//set data bits
	USART3->CR2 &= ~USART_CR2_STOP;		//set  STOP BITS
	USART3->CR1 &= ~USART_CR1_PCE;		//disable parity control
	USART3->CR1 &= ~USART_CR1_OVER8;	// set oversampling by 16
	USART3->BRR = 0x683;							//BAUD = 9600
	USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Trans/Rec enable
	USART3->CR1 |= USART_CR1_UE;
	USART3->CR1  |= USART_CR1_RXNEIE;
	while((USART3->ISR & USART_ISR_TEACK) == 0); // Ready for transmission
	while((USART3->ISR & USART_ISR_REACK) == 0); // Ready for reception	
	
	NVIC_EnableIRQ(USART3_IRQn);
}

void UART2_init(void) {
	GPIOA->MODER &= 0xFFFFFFAF; // PA2, PA3 Alt function
	GPIOA->MODER |= 0x000000A0;
	
	GPIOA->AFR[0] &= 0xFFFFF00F;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2;
	
	GPIOA->OSPEEDR |= 0x000000F0; // High speed PA2/PA3
	
	GPIOA->PUPDR |= 0x00000050; // Pull up
	GPIOA->PUPDR &= 0xFFFFFF5F;
	
	GPIOA->OTYPER &= 0xFFFFFFF3; // Push Pull
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable clock for USART2
	
	RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL); // Sys clock
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;
	
	// Init USART2
	USART2->CR1 &= ~USART_CR1_UE;			//Disable uart
	USART2->CR1 &= ~USART_CR1_M;			//set data bits
	USART2->CR2 &= ~USART_CR2_STOP;		//set  STOP BITS
	USART2->CR1 &= ~USART_CR1_PCE;		//disable parity control
	USART2->CR1 &= ~USART_CR1_OVER8;	// set oversampling by 16
	USART2->BRR = 0x683;							// Baud 9600
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Trans/Rec enable
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1  |= USART_CR1_RXNEIE;
	while((USART2->ISR & USART_ISR_TEACK) == 0); // Ready for transmission
	while((USART2->ISR & USART_ISR_REACK) == 0); // Ready for reception
	
	NVIC_EnableIRQ(USART2_IRQn);
}

void USART_Write(USART_TypeDef *USARTx, char *buffer, uint32_t nBytes) {
	uint32_t i;
		
	for (i = 0; i < nBytes; i++){
		while(!(USARTx->ISR & USART_ISR_TXE)); //Wait
		USARTx->TDR = buffer[i];
	}
	
	// Wait
	while(!(USARTx->ISR & USART_ISR_TC));
	
	// Clear Flag
	USARTx->ICR |= USART_ICR_TCCF;
}

void USART_Read(USART_TypeDef *USARTx, char *buffer, uint32_t nBytes) {
	uint32_t i;
	for (i = 0; i < nBytes; i++){
		while(!(USARTx->ISR & USART_ISR_RXNE)); // +Wait
		buffer[i] = USARTx->RDR;
		while((USARTx->ISR & USART_ISR_RXNE));
	}
}
