#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "delay.h"

void Init_GPIO(void) {
	// Enable peripheral clock of GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Configure PB0 to PB3 in output mode for the LEDs
	// Clear each field to 00. OR the masks together before complementing.
	GPIOB->MODER &= ~(GPIO_MODER_MODER0_Msk | GPIO_MODER_MODER1_Msk |
										GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
	// Now OR in value 01 shifted to correct field positions
	GPIOB->MODER |=
			_VAL2FLD(GPIO_MODER_MODER0, 1) | _VAL2FLD(GPIO_MODER_MODER1,
																								1) |
			_VAL2FLD(GPIO_MODER_MODER2, 1) | _VAL2FLD(GPIO_MODER_MODER3, 1);

	// Enable peripheral clock of GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// Configure PC13 to input mode for the switch
	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
}

void Exercise7(void) {
	Init_GPIO();
	// Turn off the LEDs - active low LEDs, so set outputs
	GPIOB->BSRR =
			GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 | GPIO_BSRR_BS_2 |
			GPIO_BSRR_BS_3;
	while (1) {
		// Wait until the switch is pressed
		while (GPIOC->IDR & GPIO_IDR_13);
		GPIOB->BSRR = GPIO_BSRR_BR_0;
		// Use delays of 450000 for about 500 ms.
		// Use delays of 30 for about 40 us.
		Delay(450000);
		GPIOB->BSRR = GPIO_BSRR_BR_1;
		Delay(450000);
		GPIOB->BSRR = GPIO_BSRR_BR_2;
		Delay(450000);
		GPIOB->BSRR = GPIO_BSRR_BR_3;

		// Wait until the switch is released 
		while (!(GPIOC->IDR & GPIO_IDR_13));

		// Turn off the LEDs
		GPIOB->BSRR =
				GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 | GPIO_BSRR_BS_2 |
				GPIO_BSRR_BS_3;
	}
}

void Init_GPIO_HAL(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable peripheral clock of GPIOB
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// Configure PB0 to PB3 in output mode for the LEDs
	GPIO_InitStructure.Pin =
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Enable peripheral clock of GPIOC
	__HAL_RCC_GPIOC_CLK_ENABLE();

	// Configure PC13 to input mode for the switch
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Exercise7_HAL(void) {
	Init_GPIO_HAL();
	// Turn off the LEDs
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	while (1) {
		// Wait until the switch is pressed
		while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		// Use delays of 450000 for about 500 ms.
		// Use delays of 30 for about 40 us.
		Delay(450000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
		Delay(450000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		Delay(450000);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

		// Wait until the switch is released
		while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));

		// Turn off the LEDs
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
}

int main(void) {
	Exercise7();
}
