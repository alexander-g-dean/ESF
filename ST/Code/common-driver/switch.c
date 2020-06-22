#include <stm32f091xc.h>
#include "switch.h"
#include "field_access.h"
#include "gpio.h"

// Start Listing 3.Init_GPIO_Switches
void Init_GPIO_Switches(void) {
	// Enable peripheral clock of GPIOC (for switches)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC7 and PC13 in input mode
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER7, ESF_GPIO_MODER_INPUT);
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT);

	// Enable pullup on each input with 01.
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR7, 1);
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13, 1);	
}
// End Listing 3.Init_GPIO_Switches

// Start Listing 3_Init_GPIO_Switches_Interrupts
void Init_GPIO_Switches_Interrupts(void) {
	// Enable peripheral clock of GPIOC (for switches)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC7 and PC13 in input mode 
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER7, ESF_GPIO_MODER_INPUT);
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT);
	// Enable pullup on each input with 01.
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR7, 1);
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13, 1);	

	// Enable peripheral clock for SYSCFG 
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	// Select Port C for bits for SW1 and SW2
	// SW1 is at Port C bit 13
	MODIFY_FIELD(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13, 2);
	// SW2 is at Port C bit 7
	MODIFY_FIELD(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI7, 2);

	// Set mask bits for inputs in EXTI_IMR
	EXTI->IMR |= MASK(SW1_POS) | MASK(SW2_POS);
	// Trigger on both rising and falling edges in EXTI_RTSR and EXTI_FTSR
	EXTI->RTSR |= MASK(SW1_POS) | MASK(SW2_POS);
	EXTI->FTSR |= MASK(SW1_POS) | MASK(SW2_POS);

	// Configure enable and mask bits for NVIC IRQ Channel for EXTI
	// Interrupt lines 7 and 13 are both serviced by EXTI4_15_IRQ
	NVIC_SetPriority(EXTI4_15_IRQn, 3);
	NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	// Optional: Configure PRIMASK in case interrupts were disabled
	__enable_irq();
}
// End Listing 3_Init_GPIO_Switches_Interrupts

