#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"

// Start Listing Init_Blue_LED_PWM
#define F_TIM_CLOCK (48UL*1000UL*1000UL)	// 48 MHz
#define PWM_FREQUENCY (500) 
#define PWM_MAX_DUTY_VALUE ( (F_TIM_CLOCK / (PWM_FREQUENCY * PWM_PRESCALER)) - 1)
#define PWM_PRESCALER (2)

void Init_Blue_LED_PWM(void) {
	// Configure PA7 (blue LED) with alternate function 1: TIM3_CH2
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER = (GPIOA->MODER & (~GPIO_MODER_MODER7)) | _VAL2FLD(GPIO_MODER_MODER7, 2);
	GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFRL7, 1);

	// Configure TIM3 counter and prescaler
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = PWM_PRESCALER - 1;
	TIM3->ARR =	PWM_MAX_DUTY_VALUE;
	TIM3->CR1 = 0; // count up

	// Configure TIM3 channel 2	
	TIM3->CCR2 = 1;								// Short on-time by default
	TIM3->CCER |= TIM_CCER_CC2P;	// active low polarity 
	TIM3->CCMR1 |= _VAL2FLD(TIM_CCMR1_OC2M, 6) | TIM_CCMR1_OC2PE;	// PWM mode 1, use preload register
	TIM3->EGR |= TIM_EGR_UG;			// Generate update
	TIM3->CCER |= TIM_CCER_CC2E; // Enable channel output on OC2	
	TIM3->BDTR |= TIM_BDTR_MOE;		// Enable main output
	
	TIM3->CR1 |= TIM_CR1_CEN;			// Enable timer 
}
// End Listing Init_Blue_LED_PWM

#if 0
/* Listing 7.12 */
TIM_OC_InitTypeDef TIM_OC;
TIM_Base_InitTypeDef TIM_InitStruct;
TIM_HandleTypeDef TIM_HandleStruct;
void HAL_Init_Blue_LED_PWM(void) {
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

	if (HAL_TIM_PWM_Init(&TIM_HandleStruct) !=
			HAL_OK
			||
			HAL_TIM_PWM_ConfigChannel(&TIM_HandleStruct,
																&TIM_OC,
																TIM_CHANNEL_2) !=
			HAL_OK
			|| HAL_TIM_PWM_Start(&TIM_HandleStruct,
													 TIM_CHANNEL_2) !=
			HAL_OK)
		while (1);
}
// End Listing Init_Blue_LED_PWM
#endif

/* Listing 7.13 */
// Start Listing LED_PWM_main
#define DIM_DELAY (100)

int main(void) {
	int pulse_width = 0, change = 1;
	volatile int32_t delay;

	Set_Clocks_To_48MHz();
	Init_Blue_LED_PWM();

	// Flash forever
	while (1) {
		TIM3->CCR2 = pulse_width;
		pulse_width += change;
		if (pulse_width <= 0)
			change = 1;
		else if (pulse_width >= PWM_MAX_DUTY_VALUE)
			change -= 1;		
		// Delay before changing pulse width again
		for (delay = 0; delay < DIM_DELAY; delay++)
			;
	}
}
// End Listing LED_PWM_main
