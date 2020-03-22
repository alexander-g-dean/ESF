#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "delay.h"
#include "rgb.h"

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

COMP_HandleTypeDef COMP_Handle;
COMP_InitTypeDef COMP_Init;

void Init_Comparator(void) {    
    COMP_Init.InvertingInput = COMP_INVERTINGINPUT_1_2VREFINT;
    COMP_Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
    COMP_Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING_FALLING;
    COMP_Handle.Instance = COMP1;
    COMP_Handle.Init = COMP_Init;
    
    HAL_COMP_Init(&COMP_Handle);
    HAL_COMP_Start_IT(&COMP_Handle);
    
    NVIC_SetPriority(ADC1_COMP_IRQn, 128);
    NVIC_ClearPendingIRQ(ADC1_COMP_IRQn);
    NVIC_EnableIRQ(ADC1_COMP_IRQn);
}

/* Listing 6.4 */
void ADC1_COMP_IRQHandler(void) {
    if (__HAL_COMP_COMP1_EXTI_GET_FLAG() != RESET) {
			  __HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
        if (HAL_COMP_GetOutputLevel(&COMP_Handle) == COMP_OUTPUTLEVEL_LOW)
            // Level of PA1 is lower than 1/2 Vref, so light green LED
            Control_RGB_LEDs(0, 1, 0);
        else 
            // Level of PA1 is higher than 1/2 Vref, so light red LED
            Control_RGB_LEDs(1, 0, 0);    
    }            
}

#ifndef USE_ANALOG_INPUT
void toggle_output(void) {
	  // Note: this part is added to test the comparator
    // An IRQ should be generated when the level of the pin changes.	
		while (1) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
			  Delay(100000);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
			  Delay(100000);
    }
}
#endif

int main(void) {
	  Init_GPIO_RGB();
		Control_RGB_LEDs(0, 0, 0);
	  Init_Comparator();
	
#ifndef USE_ANALOG_INPUT
    Init_GPIO();
		toggle_output();
#else
		while (1);
#endif
}
