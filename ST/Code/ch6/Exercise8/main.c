#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "delay.h"
#include "rgb.h"
#include "dac.h"

COMP_HandleTypeDef COMP_Handle;
COMP_InitTypeDef COMP_Init;

// LED stays blue until voltage at PA1 rises to at least 2.0 V, then 
// LED turns RED.

// Default behavior (USE_ANALOG_INPUT is defined): Connect an analog 
// voltage (0 to 3.3 V) to PA1 for the comparator to monitor.
// Can also comment out the definition of USE_ANALOG_INPUT to let a GPIO port 
// drive PA1 and simulate a changing input voltage. 
#define USE_ANALOG_INPUT 

#ifndef USE_ANALOG_INPUT
void Init_GPIO(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
	
		__HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
#endif

/* Code from Exercise 8 */
void Init_Comparator(void) {    
    COMP_Init.InvertingInput = COMP_INVERTINGINPUT_DAC1;
    COMP_Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
    COMP_Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING;
    COMP_Handle.Instance = COMP1;
    COMP_Handle.Init = COMP_Init;
    
    HAL_COMP_Init(&COMP_Handle);
    HAL_COMP_Start_IT(&COMP_Handle);
    
    NVIC_SetPriority(ADC1_COMP_IRQn, 128);
    NVIC_ClearPendingIRQ(ADC1_COMP_IRQn);
    NVIC_EnableIRQ(ADC1_COMP_IRQn);
}

void ADC1_COMP_IRQHandler(void) {
    if (__HAL_COMP_COMP1_EXTI_GET_FLAG() != RESET) {
			  __HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
        if (HAL_COMP_GetOutputLevel(&COMP_Handle) == COMP_OUTPUTLEVEL_HIGH) {
            // Level of PA1 is higher than DAC voltage, so light red LED
            Control_RGB_LEDs(1, 0, 0);    
				}
    }            
}

#ifndef USE_ANALOG_INPUT
void raise_output_after_delay(void) {
	  // This function is added to test the comparator
	  // An IRQ should be triggered when the pin is set (rising edge).
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		Delay(1000000); // Wait for a little bit
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		while (1); // Stop here
}
#endif

int main(void) {
	Init_GPIO_RGB();
	Control_RGB_LEDs(0,0,1);
	Init_DAC();
	HAL_DAC_SetValue(&DAC_HandleStruct, DAC_CHANNEL_1,
                   DAC_ALIGN_12B_R, 2482);
	Init_Comparator();
#ifndef USE_ANALOG_INPUT
	Init_GPIO();
	raise_output_after_delay();
#else
	while (1);
#endif
}
