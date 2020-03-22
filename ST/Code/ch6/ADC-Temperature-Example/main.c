#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>

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

/* Listing 6.6 */
float Measure_Temperature(void) {
	float temp;
	uint32_t n;
	
	// Start the conversion
	HAL_ADC_Start(&ADC_HandleStruct);
	
	// Wait until the conversion is done (timeout 100ms)
	HAL_ADC_PollForConversion(&ADC_HandleStruct, 100);
	
	// Read the ADC value
	n = HAL_ADC_GetValue(&ADC_HandleStruct);
	
	// Convert the voltage to Celsius
	temp = -36.9861 + n * (0.2492192 + n * (-0.36663296E-3 + 
				 n *(2.94355354E-7 + n *(-1.2099912E-10 + n * (2.439575E-14 + 
				 n* (-1.8973354E-18))))));
	
	return temp;
}

int main(void) {
	float val;
	Init_ADC();
	while (1) {
		val = Measure_Temperature();
		// use val here
	}
}
