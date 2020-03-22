#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>

void Basic_Light_Switching_Example_HAL(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable peripheral clock of GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA0 and PA1 in output mode
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure PA4 in input mode
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Turn on LED1 and turn off LED2
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

	while (1) {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)) {
			// Switch is not pressed, so light only LED2
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		} else {
			// Switch is pressed, so light only LED1
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		}
	}
}

int main(void) {
	Basic_Light_Switching_Example_HAL();
}
