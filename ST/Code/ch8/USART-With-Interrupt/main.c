#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include <string.h>
#include "clock.h"
#include "queue.h"
#include "field_access.h"

UART_HandleTypeDef UART_Handle;
/*********** USART **********/
void HAL_Init_USART2(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	UART_InitTypeDef UART_Init;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	
  // GPIO A pin 2 and 3 in alternate function 1 (USART2)
  GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF1_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 9600 baud rate, no parity, 8 bits, 1 stop bit
	UART_Init.BaudRate = 9600;
	UART_Init.Mode = UART_MODE_TX_RX; 
	UART_Init.Parity = UART_PARITY_NONE;
	UART_Init.StopBits = UART_STOPBITS_1;
	UART_Init.WordLength = UART_WORDLENGTH_8B;
	UART_Handle.Instance = USART2;
	UART_Handle.Init = UART_Init;
	
	if (HAL_UART_Init(&UART_Handle) != HAL_OK)
		while (1);
}

// Start Listing Int_USART_Init
#define F_USART_CLOCK (48UL*1000UL*1000UL) // 48 MHz
Q_T RxQ, TxQ;

void Init_USART2(int baud_rate) {
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// GPIO A pin 2 and 3 in alternate function 1 (USART2)
	// Set mode field to 2 for alternate function
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER2, 2);
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER3, 2);
	// Select USART2 (AF = 1) as alternate function
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL2, 1);
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL3, 1);
	
	USART2->BRR = F_USART_CLOCK/baud_rate; 
	MODIFY_FIELD(USART2->CR1, USART_CR1_PCE, 0);	// No parity
	MODIFY_FIELD(USART2->CR1, USART_CR1_M, 0);		// 8 data bits
	MODIFY_FIELD(USART2->CR2, USART_CR2_STOP, 0);	// 1 Stop bit
	
	// Enable interrupt generation
	MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_RXNEIE, 1);
	
	// Enable transmitter and receiver and USART
	MODIFY_FIELD(USART2->CR1, USART_CR1_TE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_RE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_UE, 1);

	// Enable USART2 interrupts in NVIC
	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);
}
// End Listing Int_USART_Init

// Start Listing Int_USART2_IRQHandler
void USART2_IRQHandler(void) {
	uint8_t ch;
	if (USART2->ISR & USART_ISR_RXNE) {	// Receive buffer not empty
		// Reading from RDR will clear RXNE
		ch = USART2->RDR;
		if (!Q_Full(&RxQ)) {
				Q_Enqueue(&RxQ, ch);
		}
	}
	if (USART2->ISR & USART_ISR_TXE) { // Transmit buffer empty
		// Writing to TDR will clear TXE
		if (!Q_Empty(&TxQ)) { // More data to send
			USART2->TDR = Q_Dequeue(&TxQ);
		} else { // Disable transmitter interrupt
			MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, 0);
		}
	}
}
// End Listing Int_USART2_IRQHandler

// Start Listing Int_USART_Q_Transmit_NB
void USART_Q_Transmit_NonBlocking(Q_T * Q, char * buf, int num_chars){
	while (num_chars > 0) { 
		while (Q_Full(Q))
			; // Temporary error: wait until queue is not full
		Q_Enqueue(Q, *buf);
		buf++;
		num_chars--;
	}
	// Ensure we'll get a transmit buffer empty interrupt
	MODIFY_FIELD(USART2->CR1, USART_CR1_TXEIE, 1);
}
// End Listing Int_USART_Q_Transmit_NB

// Start Listing Int_USART_Q_Receive_B
void USART_Q_Receive_Blocking(Q_T * Q, char * buf, int num_chars){
	// This function gets num_chars, blocking if they aren't all ready yet
	while (num_chars > 0) {
		while (Q_Empty(&RxQ)) // Block until some data is received
			;
		*buf = Q_Dequeue(&RxQ);
		num_chars--; // Got another character
		buf++; // Advance buffer pointer to space for next character
	}
}
// End Listing Int_USART_Q_Receive_B

// Start Listing Int_USART_Main
int main(void) {
	char buffer[80];
	uint8_t c;
	
	Set_Clocks_To_48MHz();
	Init_USART2(9600);
	Q_Init(&TxQ);
	Q_Init(&RxQ);
	strcpy(buffer, "Hello, world!\n\r");
	USART_Q_Transmit_NonBlocking(&TxQ, buffer, strlen(buffer));
	while (1) {
		USART_Q_Receive_Blocking(&RxQ, buffer, 1);		// Request one character 
		c = (uint8_t) buffer[0];		// Copy that character from buffer
		sprintf(buffer, "You pressed %c\n\r", c);		// Prepare response
		USART_Q_Transmit_NonBlocking(&TxQ, buffer, strlen(buffer));	
	}
}
// End Listing Int_USART_Main
