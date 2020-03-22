#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "rgb.h"
#include "clock.h"

SPI_InitTypeDef SPI_Init;
SPI_HandleTypeDef SPI_Handle;
void Init_SPI1(void) {
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

uint8_t Test_SPI_Send(uint8_t d_out) {
	uint8_t d_in;
	
	HAL_SPI_TransmitReceive(&SPI_Handle, &d_out, &d_in, 1, 100);
	
	return d_in;
}

void Test_SPI_Loopback(void) {
	uint8_t out = 'A';
	uint8_t in;
	
	while (1) {
		in = Test_SPI_Send(out);
		if (in != out)
			// Red: error, data does not match
			Control_RGB_LEDs(1, 0, 0);
		else
			// Green: data matches
			Control_RGB_LEDs(0, 1, 0);
		out++;
		if (out > 'z')
			out = 'A';
	}
}

int main(void) {
    Set_Clocks_To_48MHz();
	  Init_GPIO_RGB();
	  Init_SPI1();
	  Test_SPI_Loopback();
}
