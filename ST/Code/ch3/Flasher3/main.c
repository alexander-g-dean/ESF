#include <stm32f091xc.h>
// #include <stm32f0xx_hal.h>
#include "delay.h"
#include "switch.h"
#include "rgb.h"

#define W_DELAY_SLOW 		150
#define W_DELAY_FAST 		70
#define RGB_DELAY_SLOW 	1000
#define RGB_DELAY_FAST 	150

// Start Listing 3.7
volatile uint8_t g_flash_LED = 0;
volatile uint32_t g_w_delay = W_DELAY_SLOW;
volatile uint32_t g_RGB_delay = RGB_DELAY_SLOW;

void EXTI4_15_IRQHandler(void) {
	if (EXTI->PR & MASK(SW1_POS)) {
		// Acknowledge interrupt by writing 1 to bit, clearing it (!) 
		EXTI->PR = MASK(SW1_POS);
		// Process interrupt
		if (SWITCH_PRESSED(SW1_POS)) {
			g_flash_LED = 1;					// Flash white
		} else {
			g_flash_LED = 0;					// RGB sequence
		}
	}
	if (EXTI->PR & MASK(SW2_POS)) {
		// Acknowledge interrupt by writing 1 to bit, clearing it (!) 
		EXTI->PR = MASK(SW2_POS);
		// Process interrupt
		if (SWITCH_PRESSED(SW2_POS)) {	// Short delays
			g_w_delay = W_DELAY_FAST;
			g_RGB_delay = RGB_DELAY_FAST;
		} else {										// Long delays
			g_w_delay = W_DELAY_SLOW;
			g_RGB_delay = RGB_DELAY_SLOW;
		}
	}
	// NVIC Acknowledge interrupt
	NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
}
// End Listing 3.7

void Task_Flash(void) {
	if (g_flash_LED == 1) {				// Only run task when in flash mode
		Control_RGB_LEDs(1, 1, 1);
		Delay(g_w_delay);
		Control_RGB_LEDs(0, 0, 0);
		Delay(g_w_delay);
	}
}

void Task_RGB(void) {
	if (g_flash_LED == 0) {				//only run task when NOT in flash mode
		Control_RGB_LEDs(1, 0, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 1, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 0, 1);
		Delay(g_RGB_delay);
	}
}

// Start Listing 3.8 
void Flasher(void) {
	Init_GPIO_RGB();
	Init_GPIO_Switches_Interrupts();
	while (1) {
		Task_Flash();
		Task_RGB();
	}
}

// End Listing 3.8

int main(void) {
	Flasher();
}
