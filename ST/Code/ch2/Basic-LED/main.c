#include <stm32f091xc.h>

void Basic_Light_Switching_Example(void) {

	// Start Listing 2.4
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA5 in output mode. 
	// First clear MODER5 field to 00
	GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
	// Now OR in value 01 shifted to correct field position
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER5, 1);

	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode - clear to 00
	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
	// Don't need to set any bits, since 00 is input mode
	// End Listing 2.4
	
#if 0
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

void Basic_Light_Switching_Example_2(void) {
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure PA5 in output mode. 
	// First clear MODER5 field to 00
	GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
	// Now OR in value 01 shifted to correct field position
	GPIOA->MODER |= _VAL2FLD(GPIO_MODER_MODER5, 1);

	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode - clear to 00
	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
	// Don't need to set any bits, since 00 bits select input

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

// End Listing 2.7

void Alternate_Basic_Light_Switching_Example(void) {
	// Enable peripheral clock of GPIOA (for LD2)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA5 in output mode. 
	// First clear MODER5 field to 00
	GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
	// Now OR in value 01 shifted to correct field position
	GPIOA->MODER |=
			GPIO_MODER_MODER5_Msk & (0x1 << GPIO_MODER_MODER5_Pos);

	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode - clear to 00
	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
	// Don't need to set any bits, since 00 bits select input

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

int main(void) {
	Basic_Light_Switching_Example();
}
