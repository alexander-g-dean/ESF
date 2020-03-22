#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

#define PWM_PERIOD (48000)
#define PWM_PRESCALER (2)

/* Listing 7.12 */
TIM_OC_InitTypeDef TIM_OC;
TIM_Base_InitTypeDef TIM_InitStruct; 
TIM_HandleTypeDef TIM_HandleStruct;

void Init_Blue_LED_PWM(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
  
	// Clock gating for TIM3 and GPIO A
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
  GPIO_InitStructure.Pin = GPIO_PIN_7;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InitStruct.Period = PWM_PERIOD - 1; 
	TIM_InitStruct.Prescaler = PWM_PRESCALER - 1;
	TIM_InitStruct.CounterMode = TIM_COUNTERMODE_UP;
	TIM_OC.Pulse = 0;
	TIM_OC.OCPolarity = TIM_OCPOLARITY_LOW;
	TIM_OC.OCMode = TIM_OCMODE_PWM1;
	
	TIM_HandleStruct.Instance = TIM3;
	TIM_HandleStruct.Init = TIM_InitStruct;
	HAL_TIM_Base_Init(&TIM_HandleStruct);
	HAL_TIM_Base_Start(&TIM_HandleStruct);
	
	if (HAL_TIM_PWM_Init(&TIM_HandleStruct) != HAL_OK ||
		HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct, &TIM_OC, TIM_CHANNEL_2) != HAL_OK ||
		HAL_TIM_PWM_Start(&TIM_HandleStruct, TIM_CHANNEL_2) != HAL_OK)
			while(1);
}

/* Listing 7.13 */
int main(void) {
	uint16_t i = 0;
	volatile int32_t delay;
	
	Set_Clocks_To_48MHz();
	Init_Blue_LED_PWM();
	// Flash forever
	while (1) {
		// Brighten LED
		for (i = 0; i < PWM_PERIOD; i++) {
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct, TIM_CHANNEL_2, i);
			for (delay = 0; delay < 100; delay++);
		}
		// Dim LED
		for (i = PWM_PERIOD - 1; i > 0; i--) {
			__HAL_TIM_SET_COMPARE(&TIM_HandleStruct, TIM_CHANNEL_2, i);
			for (delay = 0; delay < 100; delay++);
		}
	}
}
