#include <stm32f091xc.h>
// #include <stm32f0xx_hal.h>
#include "clock.h"
#include "dac.h"

#define DAC_RESOLUTION (4096)

#define STEP_SIZE (16)
#define F_TIM_CLOCK (48UL*1000UL*1000UL) // 48 MHz
#define F_TIM_OVERFLOW (100UL*1000UL) // 100 kHz
#define TIM_PRESCALER (48)

#if 0
/* Listing 7.8 */
TIM_HandleTypeDef TIM_HandleStruct;
TIM_Base_InitTypeDef TIM_InitStruct; 
void HAL_Init_TIM(void) {
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
#endif

// Start Listing Basic_Init_TIM
void Init_TIM(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->ARR = F_TIM_CLOCK / (F_TIM_OVERFLOW * TIM_PRESCALER) - 1;
	TIM3->SMCR = 0;
	TIM3->CR1 = 0; // count up
	TIM3->CR2 = 0;
	TIM3->EGR = 0;
	TIM3->PSC = TIM_PRESCALER - 1;
	TIM3->DIER = TIM_DIER_UIE; // enable update interrupt
	
	NVIC_SetPriority(TIM3_IRQn, 128);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);

	TIM3->CR1 |= TIM_CR1_CEN;			// enable timer
}
// End Listing Basic_Init_TIM

#define DEBUG_ON_MSK (GPIO_BSRR_BS_7)
#define DEBUG_OFF_MSK (GPIO_BSRR_BR_7)

void Init_GPIO(void) {
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Init PA7 as digital output
	// Configure the pin as output
	// Clear mode field  to 00
	GPIOA->MODER &= ~GPIO_MODER_MODER7;
	// Set mode field to 01 for output
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER7, 1);
}

/* Listing 7.7 */
// Start Listing Basic_TIM3_IRQHandler
void TIM3_IRQHandler(void){
	static int change = STEP_SIZE;
	static uint16_t out_data = 0;
	
  // Debug signal : Entering ISR
	GPIOA->BSRR = DEBUG_ON_MSK;
	// Clear interrupt request flag
	TIM3->SR = ~TIM_SR_UIF;
	// Do ISR work
	out_data += change;
	DAC->DHR12R1 = out_data;
	if (out_data < STEP_SIZE) 
	  change = STEP_SIZE;
	else if (out_data >= DAC_RESOLUTION - 1 - STEP_SIZE)
		change = -STEP_SIZE;
	// Debug signal: exiting ISR
	GPIOA->BSRR = DEBUG_OFF_MSK;
}
// End Listing Basic_TIM3_IRQHandler

int main() {
	Set_Clocks_To_48MHz();
	Init_GPIO();
	Init_DAC();
	Init_TIM();
	while (1);
}
