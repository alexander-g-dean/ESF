#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

void Set_Clocks_To_48MHz(void) {
	RCC_ClkInitTypeDef RCC_Clk;

#if 0
	// set flash latency to 1
	FLASH->ACR = (FLASH->ACR&(~FLASH_ACR_LATENCY)) | _VAL2FLD(FLASH_ACR_LATENCY, 1); 

	RCC->CR2 |= RCC_CR2_HSI48ON; // Start HSI48
	while (!(RCC->CR2 & RCC_CR2_HSI48RDY)) // Wait for HSI48RDY
		;		
	// Clock type: SYSCLK, HCLK, PCLK1
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_HPRE)) | _VAL2FLD(RCC_CFGR_HPRE, 1); // HCLK = SYSCLK/2
		
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW)) | _VAL2FLD(RCC_CFGR_SW, 3); // Select HSI48 as system clock
	
	// Wait until HSI48 is selected
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC->CFGR & RCC_CFGR_SW))
		;
	// PCLK1 configuration
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PPRE)) | _VAL2FLD(RCC_CFGR_PPRE, 1);
#else
	__HAL_RCC_HSI48_ENABLE();
	RCC_Clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
	RCC_Clk.ClockType =
			RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
			RCC_CLOCKTYPE_PCLK1;
	RCC_Clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_Clk.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig
			(&RCC_Clk, FLASH_LATENCY_1) != HAL_OK)
		while (1);
#endif
	// disable SysTick interrupt
	SysTick->CTRL &= ~0x00000002;

}

void PLL_Modification(void) {
	// Reference Manual Code Example A.3.2
/* (1) Test if PLL is used as System clock */
/* (2) Select HSI as system clock */
/* (3) Wait for HSI switched */
/* (4) Disable the PLL */
/* (5) Wait until PLLRDY is cleared */
/* (6) Set the PLL multiplier to 6 */
/* (7) Enable the PLL */
/* (8) Wait until PLLRDY is set */
/* (9) Select PLL as system clock */
/* (10) Wait until the PLL is switched on */
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) {	/* (1) */
		RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);	/* (2) */
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {	/* (3) */
/* For robust implementation, add here time-out management */
		}
	}
	RCC->CR &= (uint32_t) (~RCC_CR_PLLON);	/* (4) */
	while ((RCC->CR & RCC_CR_PLLRDY) != 0) {	/* (5) */
/* For robust implementation, add here time-out management */
	}
	RCC->CFGR = RCC->CFGR & (~RCC_CFGR_PLLMUL) | (RCC_CFGR_PLLMUL6);	/* (6) */
	RCC->CR |= RCC_CR_PLLON;			/* (7) */
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) {	/* (8) */
/* For robust implementation, add here time-out management */
	}
	RCC->CFGR |= (uint32_t) (RCC_CFGR_SW_PLL);	/* (9) */
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {	/* (10) */
/* For robust implementation, add here time-out management */
	}
}
