#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

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

int main(void) {
	uint8_t receive_data;
	uint8_t transmit_data;
	
	Set_Clocks_To_48MHz();
	Init_USART2();
	while (1) {
		HAL_UART_Receive(&UART_Handle, &receive_data, 1, 0xFFFF);
		transmit_data = receive_data + 1;
		HAL_UART_Transmit(&UART_Handle, &transmit_data, 1, 0xFFFF);
	}
}
