#include <stm32f091xc.h>
// #include <stm32f0xx_hal.h>
#include "delay.h"
#include "switch.h"
#include "rgb.h"

#define W_DELAY_SLOW 	 	150
#define W_DELAY_FAST 		70
#define RGB_DELAY_SLOW 	1000
#define RGB_DELAY_FAST 	150

volatile uint8_t g_flash_LED = 0;
volatile uint32_t g_w_delay = W_DELAY_SLOW;
volatile uint32_t g_RGB_delay = RGB_DELAY_SLOW;

#if 0
void Init_GPIO_HAL(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure PC7 and PC13 in input mode
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStructure.Pin = SW1_POS | SW2_POS;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Init_GPIO_interrupt() {
	Init_GPIO_HAL();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = SW1_POS;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING | GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SW2_POS;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING | GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* EXTI interrupt init */
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
	HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
#endif

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

// Start Listing 3.12
void Task_RGB_FSM(void) {
	enum State { ST_RED, ST_GREEN, ST_BLUE };
	static enum State next_state;

	if (g_flash_LED == 0) {				// Only run task when NOT in flash mode
		switch (next_state) {
		case ST_RED:
			Control_RGB_LEDs(1, 0, 0);
			Delay(g_RGB_delay);
			next_state = ST_GREEN;
			break;
		case ST_GREEN:
			Control_RGB_LEDs(0, 1, 0);
			Delay(g_RGB_delay);
			next_state = ST_BLUE;
			break;
		case ST_BLUE:
			Control_RGB_LEDs(0, 0, 1);
			Delay(g_RGB_delay);
			next_state = ST_RED;
			break;
		default:
			next_state = ST_RED;
			break;
		}
	}
}
// End Listing 3.12

// Start Listing 3.13 
void Task_Flash_FSM(void) {
	enum State { ST_WHITE, ST_BLACK };
	static enum State next_state = ST_WHITE;

	if (g_flash_LED == 1) {				// Only run task when in flash mode
		switch (next_state) {
		case ST_WHITE:
			Control_RGB_LEDs(1, 1, 1);
			Delay(g_w_delay);
			next_state = ST_BLACK;
			break;
		case ST_BLACK:
			Control_RGB_LEDs(0, 0, 0);
			Delay(g_w_delay);
			next_state = ST_WHITE;
			break;
		default:
			next_state = ST_WHITE;
			break;
		}
	}
}
// End Listing 3.13

void Flasher(void) {
	Init_GPIO_RGB();
	Init_GPIO_Switches_Interrupts();
	while (1) {
		Task_Flash_FSM();
		Task_RGB_FSM();
	}
}

int main(void) {
	Flasher();
}
