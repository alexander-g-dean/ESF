#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

/* Debug bits show timing of copy operations: 
	PB1 for software copy
	PB2 for DMA copy
*/

#define ARR_SIZE (256)

uint32_t s[ARR_SIZE], d[ARR_SIZE];

DMA_HandleTypeDef DMA_handle;
void Init_DMA_To_Copy(void) {
	DMA_InitTypeDef DMA_init;
	__HAL_RCC_DMA1_CLK_ENABLE();

	DMA_init.Mode = DMA_NORMAL;
	DMA_init.Direction = DMA_MEMORY_TO_MEMORY;
	DMA_init.PeriphInc = DMA_PINC_ENABLE;
	DMA_init.MemInc = DMA_MINC_ENABLE;
	DMA_init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	DMA_init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DMA_handle.Init = DMA_init;
	DMA_handle.Instance = DMA1_Channel1;
	DMA_handle.ChannelIndex = 1;
	if (HAL_DMA_Init(&DMA_handle) != HAL_OK)
		while (1);
}

void Copy_Longwords(uint32_t *source, uint32_t *dest, uint32_t count) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_DMA_Start(&DMA_handle, (uint32_t)source, (uint32_t)dest, count);
	HAL_DMA_PollForTransfer(&DMA_handle, HAL_DMA_FULL_TRANSFER, 0xffff);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}

void Test_DMA_Copy(void) {
	uint32_t i;
	Init_DMA_To_Copy();
	for (i = 0; i < ARR_SIZE; i++) {
		s[i] = i;
		d[i] = 0;
	}
	Copy_Longwords(s, d, ARR_SIZE);
}

void Test_SW_Copy(void) {
	uint32_t * ps, *pd;
	uint32_t i;

	ps = s;
	pd = d;
	
	for (i = 0; i < ARR_SIZE; i++) {
		s[i] = i;
		d[i] = 0;
	}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	for (i=0; i<ARR_SIZE; i++) {
		*pd++ = *ps++;
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void Init_GPIO(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
  // Init PB1 as output
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Init PB2 as output
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}

int main(void) {
	uint32_t i;
	Set_Clocks_To_48MHz();
	
	Init_GPIO();

	while (1) {
		Test_SW_Copy();
		// Check that the copy has been successful
		for (i = 0; i < ARR_SIZE; i++) {
				if (d[i] != s[i])
					while (1);
		}	
		Test_DMA_Copy();
		// Check that the copy has been successful
		for (i = 0; i < ARR_SIZE; i++) {
				if (d[i] != s[i])
					while (1);
		}
	}
}
