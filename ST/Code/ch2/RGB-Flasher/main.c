// Start Listing 2.RGB_DefInit
#include <stm32f091xc.h>
#include "delay.h"

// Active-Low LED control definitions
// 0 out = LED on
// 1 out = LED off
#define LED_R_OFF_MSK	(GPIO_BSRR_BS_5)
#define LED_R_ON_MSK	(GPIO_BSRR_BR_5)
#define LED_G_OFF_MSK	(GPIO_BSRR_BS_6)
#define LED_G_ON_MSK	(GPIO_BSRR_BR_6)
#define LED_B_OFF_MSK	(GPIO_BSRR_BS_7)
#define LED_B_ON_MSK	(GPIO_BSRR_BR_7)

void RGB_Flasher_Init(void) {
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure the three pins as output
	// Clear each mode field  to 00
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk |
										GPIO_MODER_MODER6_Msk |
										GPIO_MODER_MODER7_Msk);
	// Set each mode field to 01 for output
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER5, 1) |
			_VAL2FLD(GPIO_MODER_MODER6,
							 1) | _VAL2FLD(GPIO_MODER_MODER7,
														 1);
	// Turn on LEDs 
	GPIOA->BSRR =
			LED_R_ON_MSK | LED_G_ON_MSK | LED_B_ON_MSK;
}
// End Listing 2.RGB_DefInit

// Start Listing 2.RGB_Flasher
void RGB_Flasher_Sequential(void) {
	unsigned int num = 0;

	while (1) {
		num++;
		if (num & 1)
			GPIOA->BSRR = LED_R_ON_MSK;
		else
			GPIOA->BSRR = LED_R_OFF_MSK;
		if (num & 2)
			GPIOA->BSRR = LED_G_ON_MSK;
		else
			GPIOA->BSRR = LED_G_OFF_MSK;
		if (num & 4)
			GPIOA->BSRR = LED_B_ON_MSK;
		else
			GPIOA->BSRR = LED_B_OFF_MSK;
		Delay(400000);
	}
}
// End Listing 2.RGB_Flasher

// Start Listing 2.RGB_Flasher_Par
void RGB_Flasher_Parallel(void) {
	unsigned int num = 0;

	while (1) {
		num++;
		GPIOA->ODR &= ~((0x07) << 5);	// Clear all bits in field
		GPIOA->ODR |= (~num & 0x07) << 5;	// Set given bits in field
		// Note num is complemented with ~ so a 1 bit in num creates 
		// a 0 output, turning on the LED
		Delay(400000);
	}
}
// End Listing Listing 2.RGB_Flasher_Par

int main(void) {
	RGB_Flasher_Init();
	// Comment out the unwanted flasher version
	RGB_Flasher_Sequential();
	RGB_Flasher_Parallel();
}
