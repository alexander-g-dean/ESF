// Start Listing 2.Beep
#include <stm32f091xc.h>
#include "delay.h"
#include "field_access.h"
#include "gpio.h"

#define SPKR_HIGH (GPIO_BSRR_BS_6)
#define SPKR_LOW (GPIO_BSRR_BR_6)

void Init_Speaker(void) {
	// Enable peripheral clock of GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Configure Port B pin 6 as output (mode 1)
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER6, ESF_GPIO_MODER_OUTPUT); 
	// Set to 1 initially
	GPIOB->BSRR |= SPKR_HIGH;
}

void Beep(void) {
	Init_Speaker();
	while (1) {
		// Delay is about 2 ms
		GPIOB->BSRR |= SPKR_HIGH;
		Delay(2);
		GPIOB->BSRR |= SPKR_LOW;
		Delay(2);
	}
}
// End Listing 2.Beep

int main(void) {
	Beep();
}

