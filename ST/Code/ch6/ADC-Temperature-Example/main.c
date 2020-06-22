#include <stm32f091xc.h>
#include "field_access.h"
#include "gpio.h"

// Start Listing Temp_Init_ADC
void Init_ADC(void) {
	// Enable peripheral clock of ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA0 as analog input to ADC
	// (ESF_GPIO_MODER_ANALOG = 3)
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER0, ESF_GPIO_MODER_ANALOG);
	
	// Oscillator: Enable and Select HSI14 (from STM32F0 Reference Manual, A.7.4)
	RCC->CR2 |= RCC_CR2_HSI14ON;	/* (2) Start HSI14 RC oscillator */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {	/* (3) Wait HSI14 is ready */
		/* For robust implementation, add here time-out management */
	}
	/* Select HSI14 with CKMODE=00 */
	MODIFY_FIELD(ADC1->CFGR2, ADC_CFGR2_CKMODE, 0);
	// Init ADCl
	MODIFY_FIELD(ADC1->SMPR, ADC_SMPR_SMP, 0);	// SMP = 000 for minimum sample time

	/* CFGR1: The default configuration (CFGR1 = 0) matches what we want: 
	   some features are disabled (analog watchdog, discontinous conversion 
	   mode, auto-off mode, wait conversion mode, continuous conversion mode, 
	   hardware trigger) and other features are selected: software trigger, 
	   right-aligned data, 12-bit resolution. */
	ADC1->CFGR1 = 0;

	ADC1->CHSELR = ADC_CHSELR_CHSEL0;	// Select ADC input channel 0

	// Enable ADC (from STM32F0 Reference Manual, A.7.2
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) {	/* (1) Ensure that ADRDY = 0 */
		ADC1->ISR |= ADC_ISR_ADRDY;	/* (2) Clear ADRDY */
	}
	ADC1->CR |= ADC_CR_ADEN;			/* (3) Enable the ADC */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {	/* (4) Wait until ADC ready */
		/* For robust implementation, add here time-out management */
	}
}
// End Listing Temp_Init_ADC

// Start Listing Temp_Measure_Temp
float k0 = -36.9, k1 = 0.249, k2 = -3.67E-4, k3 = 2.95E-7, 
	k4 = -1.21E-10, k5 = 2.45E-14, k6=-1.91E-18;

float Measure_Temperature(void) {
	float temp;
	int n;

	// Start the conversion
	ADC1->CR |= ADC_CR_ADSTART;
	// Busy wait until the conversion is done 
	while (!(ADC1->ISR & ADC_ISR_EOC));
	// Read the ADC value
	n = ADC1->DR;
	// Convert the voltage to Celsius
	temp = k0 + n * (k1 + n * (k2 + n * (k3 + n * (k4 + n * (k5 + n * k6)))));
	return temp;
}
// End Listing Temp_Measure_Temp

// Start Listing Temp_Main
int main(void) {
	static float val;
	Init_ADC();
	while (1) {
		val = Measure_Temperature();
		// use val here
	}
}
// End Listing Temp_Main
