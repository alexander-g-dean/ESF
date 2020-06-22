#include <stm32f091xc.h>
// #include <stm32f0xx_hal.h>
#include "delay.h"
#include "rgb.h"
#include "field_access.h"
#include "gpio.h"

// Connect an analog voltage (0 to 3.3 V) to PA1 
// for the comparator to monitor.

// Start Listing Init_Comparator
void Init_Comparator(void) {
	// Enable peripheral clock for COMP (and SYSCFG)
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA1 as analog input to comparator
	// Set bit 1 MODER field to b11 for analog
	// (ESF_GPIO_MODER_ANALOG = 3)
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER1, ESF_GPIO_MODER_ANALOG);

	// Configure PA6 as digital output from comparator
	// Set bit 6 MODER field to b10 for alternate function
	// (ESF_GPIO_MODER_ALT_FUNC = 2)
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER6, ESF_GPIO_MODER_ALT_FUNC);

	// Select CMP1_OUT as alternate function for PA6.
	// PA6 is in lower byte of port, so use AFRL, access as AFR[0] 
	MODIFY_FIELD(GPIOA->AFR[0], GPIO_AFRL_AFRL6, 7);

	// Comparator initialization
	MODIFY_FIELD(COMP->CSR, COMP_CSR_COMP1EN, 1);
	MODIFY_FIELD(COMP->CSR, COMP_CSR_COMP1INSEL, 1);
	
	// Interrupt initialization
	// Comparator 1: EXTI line 21
	EXTI->FTSR |= EXTI_FTSR_TR21;	// Enable falling trigger 
	EXTI->RTSR |= EXTI_RTSR_TR21;	// Enable rising trigger 
	EXTI->IMR |= EXTI_IMR_MR21;		// Set mask bit to enable int.

	// Enable IRQ in NVIC, shared by ADC and COMPs
	NVIC_SetPriority(ADC1_COMP_IRQn, 128);
	NVIC_ClearPendingIRQ(ADC1_COMP_IRQn);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
}
// End Listing Init_Comparator

#if 0
// Start Listing HAL 6.4
void oldADC1_COMP_IRQHandler(void) {
	if (__HAL_COMP_COMP1_EXTI_GET_FLAG() != RESET) {
		__HAL_COMP_COMP1_EXTI_CLEAR_FLAG();
		if (HAL_COMP_GetOutputLevel(&COMP_Handle) ==
				COMP_OUTPUTLEVEL_LOW)
			// Level of PA1 is lower than 1/2 Vref, so light red LED
			Control_Discrete_RGB_LEDs(1, -1, 0);
		else
			// Level of PA1 is higher than 1/2 Vref, so light blue LED
			Control_Discrete_RGB_LEDs(0, -1, 1);
	}
}
// End Listing HAL 6.4
#endif

// Start Listing COMP_ISR
void ADC1_COMP_IRQHandler(void) {
	if (EXTI->PR & EXTI_PR_PR21) {	// COMP_EXTI_LINE_COMP1
		EXTI->PR = EXTI_PR_PR21;		// Clear pending request flag
		if ((COMP->CSR & COMP_CSR_COMP1OUT) == 0)
			// Level of PA1 is lower than 1/2 Vref, so light red LED
			Control_Discrete_RGB_LEDs(1, -1, 0);
		else
			// Level of PA1 is higher than 1/2 Vref, so light blue LED
			Control_Discrete_RGB_LEDs(0, -1, 1);
	}
}
// End Listing COMP_ISR

// Start Listing COMP_Main
int main(void) {
	// Initialize red and blue LEDs for GPIO control, initially off
	Init_GPIO_Discrete_RGB(1, 0, 1, 0);
	// Initialize comparator, which will drive green LED 
	Init_Comparator();
	while (1)
		;
}
// End Listing COMP_Main

// 	EXTI_PR_PR21 vs. COMP_EXTI_LINE_COMP1

