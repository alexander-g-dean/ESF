#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "dac.h"

#define DAC_RESOLUTION (4096)

#define STEP_SIZE (1) // (16)
#define F_TIM_CLOCK (48UL*1000UL*1000UL)
#define F_TIM_OVERFLOW (100UL*1000UL)
#define TIM_PRESCALER (48)

/* Listing 7.8 */
TIM_HandleTypeDef TIM_HandleStruct;
TIM_Base_InitTypeDef TIM_InitStruct; 
void Init_TIM(void) {
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	TIM_InitStruct.Period = F_TIM_CLOCK / (F_TIM_OVERFLOW * TIM_PRESCALER) - 1;
	TIM_InitStruct.Prescaler = TIM_PRESCALER - 1;
	TIM_InitStruct.CounterMode = TIM_COUNTERMODE_UP;
	TIM_HandleStruct.Instance = TIM3;
	TIM_HandleStruct.Init = TIM_InitStruct;
	HAL_TIM_Base_Init(&TIM_HandleStruct);
	HAL_TIM_Base_Start_IT(&TIM_HandleStruct);
	
	NVIC_SetPriority(TIM3_IRQn, 128);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
}

void Init_GPIO(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* Listing 7.7 */
void TIM3_IRQHandler(void){
	static int change = STEP_SIZE;
	static uint16_t out_data = 0;
	
  // Debug signal : Entering ISR
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	
	// Do ISR work
	__HAL_TIM_CLEAR_IT(&TIM_HandleStruct, TIM_IT_UPDATE);
	out_data += change;
	if (out_data < STEP_SIZE) 
	  change = STEP_SIZE;
	else if (out_data >= DAC_RESOLUTION - STEP_SIZE)
		change = -STEP_SIZE;
	HAL_DAC_SetValue(&DAC_HandleStruct, DAC_CHANNEL_1, DAC_ALIGN_12B_R, out_data);
	// Debug signal: exiting ISR
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

int main() {
	Set_Clocks_To_48MHz();
	Init_GPIO();
	Init_DAC();
	Init_TIM();
	while (1);
}
