#include <stm32f091xc.h>
#include "field_access.h"
#include "gpio.h"

void Basic_Light_Switching_Example_1(void) {

	// Start Listing 2.4
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA5 in output mode (01=1) 
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER5, ESF_GPIO_MODER_OUTPUT);
	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode (00=0)
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT);
	// End Listing 2.4
	
#if 1
	// Start Listing 2.5
	// Definitions for bit positions
	#define LD2_POS (5)
	#define B1_POS (13)
	#define MASK(x) (1UL<<(x))
	
	// Turn off LD2 
	GPIOA->ODR &= ~MASK(LD2_POS);
	while (1) {
		// Repeatedly read switch and control LD2
		if (GPIOC->IDR & MASK(B1_POS)) {
			// 1: Switch is not pressed, so turn off LD2
			GPIOA->ODR &= ~MASK(LD2_POS);
		} else {
			// 0: Switch is pressed, so turn on LD2
			GPIOA->ODR |= MASK(LD2_POS);
		}
	}
	// End Listing 2.5
#else
	// Start Listing 2.6
	// Turn off LD2 
	GPIOA->BSRR |= GPIO_BSRR_BR_5;
	while (1) {
		if (GPIOC->IDR & GPIO_IDR_13) {
			// 1: Switch is not pressed, so turn off LD2
			GPIOA->BSRR |= GPIO_BSRR_BR_5;
		} else {
			// 0: Switch is pressed, so turn on LD2
			GPIOA->BSRR |= GPIO_BSRR_BS_5;
		}
	}
	// End Listing 2.6
#endif
}

// Start Listing 2.7
#define LD2_OFF_MSK	(GPIO_BSRR_BR_5)
#define LD2_ON_MSK	(GPIO_BSRR_BS_5)
#define B1_IN_MSK		(GPIO_IDR_13)

void Basic_Light_Switching_Example(void) {
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA5 in output mode (01=1) 
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER5, ESF_GPIO_MODER_OUTPUT);
	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode (00=0)
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT);

	// Turn off LD2 
	GPIOA->BSRR |= LD2_OFF_MSK;
	while (1) {
		if (GPIOC->IDR & B1_IN_MSK) {
			// 1: Switch is not pressed, so turn off LD2
			GPIOA->BSRR |= LD2_OFF_MSK;
		} else {
			// 0: Switch is pressed, so turn on LD2
			GPIOA->BSRR |= LD2_ON_MSK;
		}
	}
}

int main(void) {
	Basic_Light_Switching_Example();
}
// End Listing 2.7

void Alternate_Basic_Light_Switching_Example(void) {
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA5 in output mode. 
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER5, ESF_GPIO_MODER_OUTPUT);

	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode 
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_OUTPUT);


	// Turn off LD2 
	GPIOA->BSRR |= GPIO_BSRR_BR_5;
	while (1) {
		if (GPIOC->IDR & GPIO_IDR_13) {
			// 1: Switch is not pressed, so turn off LD2
			GPIOA->BSRR |= GPIO_BSRR_BR_5;
		} else {
			// 0: Switch is pressed, so turn on LD2
			GPIOA->BSRR |= GPIO_BSRR_BS_5;
		}
	}
}
