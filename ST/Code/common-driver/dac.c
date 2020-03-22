#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "dac.h"

DAC_HandleTypeDef DAC_HandleStruct;

void Init_DAC(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    DAC_ChannelConfTypeDef DAC_Conf;
		
	__HAL_RCC_DAC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
    // Init PA4 as analog output
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Init DAC1
	  DAC_HandleStruct.Instance = DAC1;
    DAC_Conf.DAC_Trigger = DAC_TRIGGER_NONE;
    DAC_Conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_Init(&DAC_HandleStruct);
	  HAL_DAC_ConfigChannel(&DAC_HandleStruct, &DAC_Conf, DAC_CHANNEL_1);
    HAL_DAC_Start(&DAC_HandleStruct, DAC_CHANNEL_1);
}
