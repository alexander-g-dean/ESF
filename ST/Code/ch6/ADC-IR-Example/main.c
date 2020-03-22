#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "delay.h"
#include "rgb.h"

#define RED (0)
#define GREEN (1)
#define BLUE (2)
#define NUM_RANGE_STEPS (6)

/* Listing 6.7 */
#define T_DELAY_ON (1000)
#define T_DELAY_OFF (1000)
#define NUM_SAMPLE_TO_AVG (10)

ADC_HandleTypeDef ADC_HandleStruct;
void Init_ADC(void) {
	  GPIO_InitTypeDef GPIO_InitStruct;
		ADC_InitTypeDef ADC_InitStruct;
	  ADC_ChannelConfTypeDef ADC_ChannelStruct;
	
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_ADC1_CLK_ENABLE();
	
		RCC_OscInitTypeDef   RCC_OscInitStructure;
    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI14;
    RCC_OscInitStructure.HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT;
    RCC_OscInitStructure.HSI14State = RCC_HSI14_ADC_CONTROL;
    HAL_RCC_OscConfig(&RCC_OscInitStructure); 
	
	  // Init PA0 as analog
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  // Init ADC
	  ADC_InitStruct.DiscontinuousConvMode = ENABLE;
	  ADC_InitStruct.ContinuousConvMode = DISABLE;
		ADC_InitStruct.ExternalTrigConv = ADC_SOFTWARE_START;
	  ADC_InitStruct.DataAlign = ADC_DATAALIGN_RIGHT;
	  ADC_InitStruct.Resolution = ADC_RESOLUTION_12B;
		ADC_InitStruct.EOCSelection = ADC_EOC_SINGLE_CONV;
		ADC_InitStruct.SamplingTimeCommon = ADC_SAMPLETIME_1CYCLE_5;
		ADC_ChannelStruct.Channel = ADC_CHANNEL_0;
	  ADC_HandleStruct.Instance = ADC1;
	  ADC_HandleStruct.Init = ADC_InitStruct;
		HAL_ADC_Init(&ADC_HandleStruct);
		HAL_ADC_ConfigChannel(&ADC_HandleStruct, &ADC_ChannelStruct);
}

/* Listing 6.8 */
void Init_IR_LED(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Init PB0 as digital output
	__HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* Listing 6.9 */
void Control_IR_LED(unsigned int led_on) {
	if (led_on)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}	

unsigned int Measure_IR(void) {
	uint32_t n;
	// Start the conversion
	HAL_ADC_Start(&ADC_HandleStruct);
	
	// Wait until the conversion is done (timeout 100ms)
	HAL_ADC_PollForConversion(&ADC_HandleStruct, 100);
	
	// Read the ADC value
	n = HAL_ADC_GetValue(&ADC_HandleStruct);
	
	// Complement result since voltage falls with increasing IR
	// but we want result to rise with increasing IR
	return 0xfff-n;
}

int Threshold[NUM_RANGE_STEPS] = {500, 200, 80, 30, 5, 0};
const int Colors[NUM_RANGE_STEPS][3] = {{1, 1, 1}, //white
																				{1, 0, 1}, // magenta
																				{1, 0, 0}, // red
																				{1, 1, 0}, // yellow
																				{0, 0, 1}, // blue
																				{0, 1, 0}}; // green

void Display_Range(int b) {
	unsigned int i;
	
	for (i = 0; i < NUM_RANGE_STEPS - 1; i++) {
		if (b > Threshold[i]) 
			break;
	}
	Control_RGB_LEDs(Colors[i][RED], Colors[i][GREEN], Colors[i][BLUE]);
}	

int main(void) {
	static int on_brightness = 0;
	static int off_brightness = 0;
	static int avg_diff;
	static int diff;
	unsigned int n;
	
	Init_ADC();
	Init_GPIO_RGB();
	Init_IR_LED();
	Control_RGB_LEDs(0, 0, 0);
	
	while (1) {
		diff = 0;
		for (n = 0; n < NUM_SAMPLE_TO_AVG; n++) {
			// Measure IR level with IRLED off
			Control_IR_LED(0);
			Delay(T_DELAY_OFF);
			off_brightness = Measure_IR();
			
			// Measure IR level with IRLED on
			Control_IR_LED(1);
			Delay(T_DELAY_ON);
			on_brightness = Measure_IR();
			
			// Compute difference
			diff += on_brightness - off_brightness;
		}
		// Compute average difference
		avg_diff = diff / NUM_SAMPLE_TO_AVG;
		// Light RGB LED according to range
		Display_Range(avg_diff);
	}
}
