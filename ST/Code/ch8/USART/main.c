#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "delay.h"
#include "field_access.h"

#define F_USART_CLOCK (48UL*1000UL*1000UL) // 48 MHz


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

// Start Listing Poll_USART_Init
#define F_USART_CLOCK (48UL*1000UL*1000UL) // 48 MHz

void Init_USART2(void) {
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// GPIO A pin 2 and 3 in alternate function 1 (USART2)
	// Set mode field to 2 for alternate function
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER2, 2);
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER3, 2);
	// Select USART2 (AF = 1) as alternate function
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL2, 1);
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFSEL3, 1);
	
	// 9600 baud rate
	USART2->BRR = F_USART_CLOCK/9600; 
	// No parity
	MODIFY_FIELD(USART2->CR1, USART_CR1_PCE, 0);
	// 8 data bits
	MODIFY_FIELD(USART2->CR1, USART_CR1_M, 0);
	// 1 Stop bit
	MODIFY_FIELD(USART2->CR2, USART_CR2_STOP, 0);
	
	// Enable transmitter and receiver and USART
	MODIFY_FIELD(USART2->CR1, USART_CR1_TE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_RE, 1);
	MODIFY_FIELD(USART2->CR1, USART_CR1_UE, 1);
}
// End Listing Poll_UART_Init

// Start Listing Poll_UART
void UART_Transmit(uint8_t data) {
	while ((USART2->ISR & USART_ISR_TXE) == 0)
		;
	USART2->TDR = data;
}
uint8_t UART_Receive(void) {
	while ((USART2->ISR & USART_ISR_RXNE) == 0)
		;
	return USART2->RDR;
}
int main(void) {
	uint8_t receive_data, transmit_data;
	
	Set_Clocks_To_48MHz();
	Init_USART2();
	UART_Transmit('!'); 
	while (1) {
		receive_data = UART_Receive();
		transmit_data = receive_data + 1;
		UART_Transmit(transmit_data);
	}
}
// End Listing Poll_UART
