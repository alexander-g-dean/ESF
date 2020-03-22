#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "rgb.h"

#define M_PI (3.14159)
#define ANEM_R_MM (70)
#define ANEM_CLK_FREQ (375000)
#define KTS_PER_MM_S (0.00194384)

TIM_HandleTypeDef TIM_HandleStruct;
TIM_Base_InitTypeDef TIM_InitStruct; 

volatile uint32_t g_anem_period = 0;
volatile uint32_t g_new_data = 0;
volatile float v_w = 0;


void Init_GPIO(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* Listing 7.9 */
TIM_IC_InitTypeDef TIM_IC;
void Init_TIM_IC(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Clock gating for TIM3 and GPIO B
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
  GPIO_InitStructure.Pin = GPIO_PIN_4;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_InitStruct.Period = 0xFFFF;
	TIM_InitStruct.Prescaler = 127;
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

/* Listing 7.10 */
void TIM3_IRQHandler(void) {
	static volatile uint32_t overflows = 0;
	static volatile uint32_t prev_count = 0;
	static volatile uint32_t timer_val;
	
	// Debug signal : Entering ISR
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	
	if (__HAL_TIM_GET_FLAG(&TIM_HandleStruct, TIM_FLAG_UPDATE))
		// Overflows detected
	  overflows++;
	if (__HAL_TIM_GET_FLAG(&TIM_HandleStruct, TIM_FLAG_CC1)) {
		timer_val = HAL_TIM_ReadCapturedValue(&TIM_HandleStruct, TIM_CHANNEL_1);
	  // Each overflow is 2^16 counts
		timer_val |= overflows << 16;
		g_anem_period = timer_val - prev_count;
		prev_count = timer_val;
		g_new_data = 1;
	}
	if (overflows > (prev_count >> 16) + 7) {
		// Almost no wind, so zero out period
		g_anem_period = 0;
		g_new_data = 1;
	}	

	__HAL_TIM_CLEAR_IT(&TIM_HandleStruct, TIM_IT_UPDATE);
	__HAL_TIM_CLEAR_IT(&TIM_HandleStruct, TIM_IT_CC1);
	
	// Debug signal: exiting ISR
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
}

/* Listing 7.11 */
float Calculate_Windspeed_kt(void) {
	float v;
	if (g_anem_period > 0)
		v = KTS_PER_MM_S * 2 * M_PI * ANEM_R_MM * ANEM_CLK_FREQ / g_anem_period;
	else
		v = 0;
	return v;
}

void RGB_Windspeed(float v) {
	int r, g, b;
	r = g = b = 0;
	if (v < 0.1)
		r = 0;
	else if (v < 1)
		r = 1;
	else if (v < 3)
		r = g = 1;
	else if (v < 5)
		g = 1;
	else
		r = g = b = 1;
	Control_RGB_LEDs(r, g, b);
}

int main(void) {
	
  Set_Clocks_To_48MHz();
	Init_GPIO();
	Init_TIM_IC();
	Init_GPIO_RGB();
	Control_RGB_LEDs(0,0,0);
	
	while (1) {		
#if 0
		v_w = Calculate_Windspeed_kt();
		RGB_Windspeed(v_w);
#else
		if (g_new_data) {
			g_new_data = 0;
			v_w = Calculate_Windspeed_kt();
			RGB_Windspeed(v_w);
		}
#endif
	}
}
