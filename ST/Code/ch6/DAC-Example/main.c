#include <stm32f091xc.h>
#include "dac.h"

void Init_PA9_Debug(void) {
		// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure the pin as output
	// Clear mode field to 00
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_Msk);
	// Set mode field to 01 for output
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER9, 1);

	// Clear output
	GPIOA->BSRR = GPIO_BSRR_BR_9;
}
// Start Listing 6.2
#define MAX_DAC_CODE 4095

void Triangle_Output(void) {
	int i = 0;
	int change = 1;

	while (1) {
		DAC->DHR12R1 = i;
		i += change;
		if (i <= 0)
			change = 1;
		else if (i >= MAX_DAC_CODE - 1)
			change = -1;
	}
}
// End Listing 6.2

int main(void) {
//	Init_PA9_Debug();
	Init_DAC();
	Triangle_Output();
}
