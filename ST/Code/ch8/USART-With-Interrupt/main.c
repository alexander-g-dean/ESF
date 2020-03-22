#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include <string.h>
#include "clock.h"
#include "queue.h"

UART_HandleTypeDef UART_Handle;
/*********** USART **********/
void Init_USART2(void) {
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

void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(&UART_Handle);
}

Q_T RxQ;
void Init_USART2_interrupt(void) {
	Q_Init(&RxQ);
		
	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);
}

volatile unsigned int tx_busy;
uint8_t receive_data;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
			tx_busy = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (!Q_Full(&RxQ)) 
			Q_Enqueue(&RxQ, receive_data);
}

int main(void) {
	char buffer[80];
	uint8_t c;
	
	Set_Clocks_To_48MHz();
	Init_USART2();
	Init_USART2_interrupt();
	while (1) {
		HAL_UART_Receive_IT(&UART_Handle, &receive_data, 1);
		while (Q_Empty(&RxQ));
		
		if (!tx_busy) {
			c = Q_Dequeue(&RxQ);
		
			// Non blocking transmit
			sprintf(buffer, "You pressed %c\n\r", c);
			tx_busy = 1;
			HAL_UART_Transmit_IT(&UART_Handle, (uint8_t *)buffer, strlen(buffer));
		}
	}
}
