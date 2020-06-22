#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "rgb.h"
#include "clock.h"
#include "field_access.h"
#include "gpio.h"

// Start Listing SPI_SPI_Init
void Init_SPI1(void) {
  // Clock gating for SPI1 and GPIO A and B
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
  // GPIO A pin 15 in alternate function 0 (SPI1) for NSS
	// Set mode field to 2 for alternate function
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER15, ESF_GPIO_MODER_ALT_FUNC);
	// Select SPI1 (AF = 0) for alternate function
	MODIFY_FIELD(GPIOA->AFR[1], GPIO_AFRH_AFSEL15, 0);
  // GPIO B pin 3, 4, 5 in alternate function 0 (SPI1) for SCK, MISO, MOSI
	// Set each mode field to 2 for alternate function
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER3, 2);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER4, 2);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER5, 2);
	// Select SPI1 (AF = 0) for alternate function
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL3, 0);
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL4, 0);
	MODIFY_FIELD(GPIOB->AFR[0], GPIO_AFRL_AFSEL5, 0);

	// Clock is divided by 16 (2^(BR+1))
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_BR, 3);
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_MSTR, 1);	// Master mode
	// Select first edge sample, active high clock
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_CPHA, 0);
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_CPOL, 1);
	// Data is LSB first
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_LSBFIRST, 1);
	// Data is 8 bits long
	MODIFY_FIELD(SPI1->CR2, SPI_CR2_DS, 7);
	// RXNE when at least 1 byte in RX FIFO
	MODIFY_FIELD(SPI1->CR2, SPI_CR2_FRXTH, 1);
	// Have NSS pin asserted automatically
	MODIFY_FIELD(SPI1->CR2, SPI_CR2_NSSP, 1);
	// Enable SPI
	MODIFY_FIELD(SPI1->CR1, SPI_CR1_SPE, 1);
}
// End Listing SPI_SPI_Init

SPI_InitTypeDef SPI_Init;
SPI_HandleTypeDef SPI_Handle;
void HAL_Init_SPI1(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

  // Clock gating for SPI1 and GPIO B
  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  // GPIO B pin 3, 4, 5 in alternate function 0 (SPI1)
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; //SCK, MISO, MOSI
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Clock is divided by 4
	SPI_Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	// Select first edge sample, active high clock
	SPI_Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI_Init.CLKPolarity = SPI_POLARITY_HIGH;
	// Data is 8 bits long, LSB first
	SPI_Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_Init.FirstBit = SPI_FIRSTBIT_LSB;
	// Master mode
	SPI_Init.Mode = SPI_MODE_MASTER;
	// Duplex communication
	SPI_Init.Direction = SPI_DIRECTION_2LINES;
	// NSS pin is asserted automatically
	SPI_Init.NSS = SPI_NSS_HARD_OUTPUT;
	
	SPI_Handle.Init = SPI_Init;
	SPI_Handle.Instance = SPI1;
	
	HAL_SPI_Init(&SPI_Handle);
}

// Start Listing SPI_Send_Receive_Byte
uint8_t SPI_Send_Receive_Byte(uint8_t d_out) {
	uint8_t d_in;
	// Wait until transmitter buffer is empty
	while ((SPI1->SR & SPI_SR_TXE) == 0)
		; 
	// Transmit d_out
	// Must tell compiler to use a byte write (not half-word)
	// by casting SPI1->DR into a pointer to a byte (uint8_t).
	// See STM32F0 Snippets (SPI_01_FullDuplexCommunications).
	*((uint8_t *)&(SPI1->DR)) = d_out;
	// Wait until receiver is not empty
	while ((SPI1->SR & SPI_SR_RXNE) == 0)
		;
	// Get d_in
	d_in = (uint8_t) SPI1->DR;
	return d_in;
};
// End Listing SPI_Send_Receive_Byte

uint8_t HAL_Test_SPI_Send(uint8_t d_out) {
	uint8_t d_in;
	
	HAL_SPI_TransmitReceive(&SPI_Handle, &d_out, &d_in, 1, 100);
	return d_in;
}

// Start Listing SPI_Test_SPI_Loopback
void Test_SPI_Loopback(void) {
	uint8_t out = 'A';
	uint8_t in;
	
	while (1) {
		in = SPI_Send_Receive_Byte(out);
		if (in != out)	// Red: error, data does not match
			Control_RGB_LEDs(1, 0, 0);
		else  					// Green: data matches
			Control_RGB_LEDs(0, 1, 0);
		out++;
		if (out > 'z')
			out = 'A';
	}
}
// End Listing SPI_Test_SPI_Loopback

// Start Listing SPI_main
int main(void) {
	Set_Clocks_To_48MHz();
	Init_GPIO_RGB();
	Init_SPI1();
	Test_SPI_Loopback();
}
// End Listing SPI_main
