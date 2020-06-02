#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "rgb.h"

volatile uint32_t g_anem_period = 0;
volatile uint32_t g_new_data = 0;
volatile float v_w = 0;

#define MIN_SPEED_TIMEOUT (6)

void Init_PA9_Debug(void) {
		// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure the three pins as output
	// Clear mode field to 00
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_Msk);
	// Set mode field to 01 for output
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER9, 1);

	// Clear output
	GPIOA->BSRR = GPIO_BSRR_BR_9;
}

#if 0
void HAL_Init_PA9_Debug(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif

// Start Listing IC_Init
#define F_TIM_CLOCK (48UL*1000UL*1000UL) // 48 MHz
#define TIM_PRESCALER (4800)

void Init_TIM_IC(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	// Use alternate function for PB4
	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER4)) | _VAL2FLD(GPIO_MODER_MODER4, 2);
	// Connect PB4 to TIM3_CH1
	GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 1);
	
	// Configure Time-Base
	TIM3->ARR = 0xffff;
	TIM3->SMCR = 0;
	TIM3->CR1 = 0; // count up
	TIM3->CR2 = 0;
	TIM3->EGR = 0;
	TIM3->PSC = TIM_PRESCALER - 1;
	
	// Select input TI1, enable CC1, rising edge detection
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0; 
	TIM3->CCER |= TIM_CCER_CC1E;
	// Enable interrupts for capture and overflow/update
	TIM3->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE; 
	TIM3->SR = 0; 

	NVIC_SetPriority(TIM3_IRQn, 128);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);

	//	Enable TIM3
	TIM3->CR1 |= TIM_CR1_CEN; 
}
// End Listing IC_Init

#if 0
/* Listing 7.9 */
TIM_HandleTypeDef TIM_HandleStruct;
TIM_Base_InitTypeDef TIM_InitStruct; 

TIM_IC_InitTypeDef TIM_IC;
void HAL_Init_TIM_IC(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Clock gating for TIM3 and GPIO B
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
  GPIO_InitStructure.Pin = GPIO_PIN_4;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_InitStruct.Period = 0xFFFF;
	TIM_InitStruct.Prescaler = TIM_PRESCALER-1;
	TIM_InitStruct.CounterMode = TIM_COUNTERMODE_UP;
	TIM_IC.ICFilter = 0x0;
	TIM_IC.ICPolarity = TIM_ICPOLARITY_RISING;
	TIM_IC.ICPrescaler = TIM_ICPSC_DIV1;
	TIM_IC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	
	TIM_HandleStruct.Instance = TIM3;
	TIM_HandleStruct.Init = TIM_InitStruct;
	HAL_TIM_Base_Init(&TIM_HandleStruct);
	HAL_TIM_Base_Start_IT(&TIM_HandleStruct);
	if (HAL_TIM_IC_Init(&TIM_HandleStruct) != HAL_OK ||
		  HAL_TIM_IC_ConfigChannel(&TIM_HandleStruct, &TIM_IC, TIM_CHANNEL_1) != HAL_OK ||
		  HAL_TIM_IC_Start_IT(&TIM_HandleStruct, TIM_CHANNEL_1) != HAL_OK)
		while (1);
	NVIC_SetPriority(TIM3_IRQn, 128);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
}
#endif

/* Listing 7.10 */
// Start Listing IC_TIM3_IRQ
void TIM3_IRQHandler(void) {
	static volatile uint32_t overflows = 0;
	static volatile uint32_t timer_val, prev_timer_val = 0;
	static uint32_t overflows_since_last_edge = 0;
	int32_t new_edge_capture = 0, new_overflow = 0;
	uint32_t extra_overflows = 0;

	// Debug signal : Entering ISR
	GPIOA->BSRR = GPIO_BSRR_BS_9;

	if (TIM3->SR & TIM_SR_CC1IF) { // Rising edge
		TIM3->SR = ~TIM_SR_CC1IF;
		timer_val = TIM3->CCR1;
		new_edge_capture = 1;
	}
	if (TIM3->SR & TIM_SR_UIF) { // Update/Timer overflow
		TIM3->SR = ~TIM_SR_UIF;
		overflows++;
		new_overflow = 1;
		overflows_since_last_edge++;
		if ((!new_edge_capture) && (overflows_since_last_edge > MIN_SPEED_TIMEOUT)) {
			g_new_data = 1;
			g_anem_period = 0;
		}
	}
	if (new_edge_capture) {
		if (new_overflow && (TIM3->CCR1 > 0x8000)) { 
			// overflowed after capture, so remove for this measurement
			extra_overflows = 1;
		}
		// calculate period
		timer_val |= (overflows - extra_overflows) << 16;
		g_anem_period = timer_val - prev_timer_val;
		prev_timer_val = timer_val;
		g_new_data = 1;
		overflows_since_last_edge = 0;
	}
	TIM3->SR = ~0; // clear all interrupt flags
	// Debug signal: exiting ISR
	GPIOA->BSRR = GPIO_BSRR_BR_9;
}
// End Listing IC_TIM3_IRQ

/* Listing 7.11 */
// Start Listing IC_Calculate
#define M_PI (3.14159)
#define ANEM_R_MM (70)
#define ANEM_CLK_FREQ (F_TIM_CLOCK/TIM_PRESCALER)
#define KTS_PER_MM_S (0.00194384)

float Calculate_Windspeed_kt(uint32_t period) {
	float v;
	if (period > 0)
		v = KTS_PER_MM_S * 2 * M_PI * ANEM_R_MM * ANEM_CLK_FREQ / g_anem_period;
	else
		v = 0;
	return v;
}
// End Listing IC_Calculate

// Start Listing IC_RGB
void RGB_Windspeed(float v) {
	int r, g, b;
	r = g = b = 0;
	if (v < 0.1) // off
		r = 0;
	else if (v < 1) // red
		r = 1;
	else if (v < 3) // yellow
		r = g = 1;
	else if (v < 5) // green
		g = 1;
	else // white
		r = g = b = 1;
	Control_RGB_LEDs(r, g, b);
}
// End Listing IC_RGB

// Start Listing IC_main
int main(void) {
	uint32_t period;
	
  Set_Clocks_To_48MHz();
	Init_PA9_Debug();
	Init_TIM_IC();
	Init_GPIO_RGB();
	Control_RGB_LEDs(0,0,0);
	
	while (1) {		
		if (g_new_data) {
			period = g_anem_period;
			g_new_data = 0;
			v_w = Calculate_Windspeed_kt(period);
			RGB_Windspeed(v_w);
		}
	}
}
// End Listing IC_main