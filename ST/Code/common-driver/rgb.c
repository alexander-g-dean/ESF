// Start Listing 3.2b
#include <stm32f091xc.h>
#include "rgb.h"

void Init_GPIO_RGB(void) {
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure the three pins as output
	// Clear each mode field  to 00
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk |
										GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk);
	// Set each mode field to 01 for output
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER5, 1) |
			_VAL2FLD(GPIO_MODER_MODER6, 1) | _VAL2FLD(GPIO_MODER_MODER7, 1);

	// Turn on LEDs 
	GPIOA->BSRR = LED_R_ON_MSK | LED_G_ON_MSK | LED_B_ON_MSK;
}

void Control_RGB_LEDs(int r_on, int g_on, int b_on) {
	GPIOA->BSRR = (r_on) ? LED_R_ON_MSK : LED_R_OFF_MSK;
	GPIOA->BSRR = (g_on) ? LED_G_ON_MSK : LED_G_OFF_MSK;
	GPIOA->BSRR = (b_on) ? LED_B_ON_MSK : LED_B_OFF_MSK;
}
// End Listing 3.2b

void Control_RGB_LEDs_0(int r_on, int g_on, int b_on) {
	if (r_on)
		GPIOA->BSRR = LED_R_ON_MSK;
	else
		GPIOA->BSRR = LED_R_OFF_MSK;
	if (g_on)
		GPIOA->BSRR = LED_G_ON_MSK;
	else
		GPIOA->BSRR = LED_G_OFF_MSK;
	if (b_on)
		GPIOA->BSRR = LED_B_ON_MSK;
	else
		GPIOA->BSRR = LED_B_OFF_MSK;
}

