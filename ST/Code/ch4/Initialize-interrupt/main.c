#include <stm32f091xc.h>
#include "field_access.h"
#include "gpio.h"
#include "switch.h"

#define W_DELAY_SLOW 4
#define W_DELAY_FAST 2
#define RGB_DELAY_SLOW 40
#define RGB_DELAY_FAST 10

void Init_GPIO_Switches_Interrupts(void) {
// Start Listing 1_Init_GPIO_Switches_Interrupts
	// Enable peripheral clock of GPIOC (for switches)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC7 and PC13 in input mode 
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER7, ESF_GPIO_MODER_INPUT);
	MODIFY_FIELD(GPIOC->MODER, GPIO_MODER_MODER13, ESF_GPIO_MODER_INPUT);
	// Enable pullup on each input with 01.
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR7, 1);
	MODIFY_FIELD(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13, 1);	
// End Listing 1_Init_GPIO_Switches_Interrupts

// Start Listing 2_Init_GPIO_Switches_Interrupts
	// Enable peripheral clock for SYSCFG 
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	// Select Port C for bits for SW1 and SW2
	// SW1 is at Port C bit 13
	MODIFY_FIELD(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13, 2);
	// SW2 is at Port C bit 7
	MODIFY_FIELD(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI7, 2);
// End Listing 2_Init_GPIO_Switches_Interrupts
	
// Start Listing 3_Init_GPIO_Switches_Interrupts
	// Set mask bits for inputs in EXTI_IMR
	EXTI->IMR |= MASK(SW1_POS) | MASK(SW2_POS);
	// Trigger on both rising and falling edges in EXTI_RTSR and EXTI_FTSR
	EXTI->RTSR |= MASK(SW1_POS) | MASK(SW2_POS);
	EXTI->FTSR |= MASK(SW1_POS) | MASK(SW2_POS);
// End Listing 3_Init_GPIO_Switches_Interrupts

// Start Listing 4_Init_GPIO_Switches_Interrupts
	// Configure enable and mask bits for NVIC IRQ Channel for EXTI
	// Interrupt lines 7 and 13 are both serviced by EXTI4_15_IRQ
	NVIC_SetPriority(EXTI4_15_IRQn, 3);
	NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
// End Listing 4_Init_GPIO_Switches_Interrupts

	/* Optional: Configure PRIMASK in case interrupts were disabled */
	__enable_irq();
}

// Start Listing 4_Int_Handler
volatile uint8_t g_flash_LED = 0;
volatile uint32_t g_w_delay = W_DELAY_SLOW;
volatile uint32_t g_RGB_delay = RGB_DELAY_SLOW;

void EXTI4_15_IRQHandler(void) {
  if ((EXTI->PR & SW1_POS) != 0) {
		EXTI->PR = SW1_POS;  // clear pending request
			if (SWITCH_PRESSED(SW1_POS)) {
				g_flash_LED = 1; // Flash white
			} else {
				g_flash_LED = 0; // RGB sequence
			}
    } 
	if ((EXTI->PR & SW2_POS) != 0) {
		EXTI->PR = SW2_POS; // clear pending request
		if (SWITCH_PRESSED(SW2_POS)) {
			// Short delays
			g_w_delay = W_DELAY_FAST;
			g_RGB_delay = RGB_DELAY_FAST;
		} else {
			// Long delays
			g_w_delay = W_DELAY_SLOW;
			g_RGB_delay = RGB_DELAY_SLOW;
		}
	}
	// Clear all other pending requests for this handler
	EXTI->PR = 0x0000fff0; 
}
// End Listing 4_Int_Handler

int main(void) {
	Init_GPIO_Switches_Interrupts();
	while (1);
}
