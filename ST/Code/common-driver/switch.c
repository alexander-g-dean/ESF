#include <stm32f091xc.h>
#include "switch.h"

// Start Listing 3.Init_GPIO_Switches
void Init_GPIO_Switches(void) {
	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC13 in input mode - clear to 00
	GPIOC->MODER &= ~(GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER13_Msk);
	// Don't need to set any bits, since 00 bits select input

	// Enable pullup on each input with 01. First clear the field bits
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR7_Msk | GPIO_PUPDR_PUPDR13_Msk);
	// Set bits to 01 to select pull-up
	GPIOC->PUPDR |=
			_VAL2FLD(GPIO_PUPDR_PUPDR7, 1) | _VAL2FLD(GPIO_PUPDR_PUPDR13,
																							1);
}
// End Listing 3.Init_GPIO_Switches

// Start Listing 3.Init_GPIO_Switches_Interrupts
void Init_GPIO_Switches_Interrupts(void) {
	// Enable peripheral clock of GPIOC (for Switch B1)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// Enable peripehral clock for SYSCFG to select EXTI bits
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

	// Configure PC13 in input mode - clear to 00
	GPIOC->MODER &= ~(GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER13_Msk);
	// Don't need to set any bits, since 00 bits select input

	// Enable pullup on each input with 01. First clear the field bits
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR7_Msk | GPIO_PUPDR_PUPDR13_Msk);
	// Then set bits to 01 to select pull-up
	GPIOC->PUPDR |=
			_VAL2FLD(GPIO_PUPDR_PUPDR7, 1) | _VAL2FLD(GPIO_PUPDR_PUPDR13,
																								1);

	// Select Port C for bits for SW1 and SW2
	// SW1 is at Port C bit 13
	SYSCFG->EXTICR[3] &= SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// SW2 is at Port C bit 7
	SYSCFG->EXTICR[1] &= SYSCFG_EXTICR2_EXTI7;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PC;

	// Set mask bits for inputs in EXTI_IMR
	EXTI->IMR |= MASK(SW1_POS) | MASK(SW2_POS);

	// Trigger on both rising and falling edges in EXTI_RTSR and EXTI_FTSR
	EXTI->RTSR |= MASK(SW1_POS) | MASK(SW2_POS);
	EXTI->FTSR |= MASK(SW1_POS) | MASK(SW2_POS);

	// Configure enable and mask bits for NVIC IRQ Channel for EXTI
	// Interrupt lines 7 and 13 are serviced by EXTI4_15_IRQ
	NVIC_SetPriority(EXTI4_15_IRQn, 3);
	NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}
// End Listing 3.Init_GPIO_Switches_Interrupts
