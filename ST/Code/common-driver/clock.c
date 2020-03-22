#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

void Set_Clocks_To_48MHz(void) {
	RCC_ClkInitTypeDef  RCC_Clk;

	__HAL_RCC_HSI48_ENABLE();
	
	RCC_Clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
	RCC_Clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_Clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_Clk.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_Clk, FLASH_LATENCY_1) != HAL_OK)
		while (1);
	// disable SysTick interrupt
	SysTick->CTRL &= ~0x00000002;
	
}
