#include <stm32f091xc.h>
#include "delay.h"
#include "switch.h"
#include "rgb.h"

#define W_DELAY_SLOW 	 	150
#define W_DELAY_FAST 		70
#define RGB_DELAY_SLOW 	1000
#define RGB_DELAY_FAST 	150

volatile uint8_t g_flash_LED = 0;
volatile uint8_t g_flash_LED_changed = 1;
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
		g_flash_LED_changed = 1;
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


unsigned int TIM_Expired(void) {
	return TIM3->SR & TIM_SR_UIF;
}

void Stop_TIM(void) {
	TIM3->SR &= ~TIM_SR_UIF; // Must clear manually!
	TIM3->CR1 &= ~TIM_CR1_CEN;
}

#define TIM_PRESCALER (4800)

void Start_TIM(uint32_t delay) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	// Stop timer in case it was already running
	Stop_TIM();
	// Initialize timer
	TIM3->SMCR = 0;
	TIM3->CR1 |= TIM_CR1_DIR | TIM_CR1_OPM | TIM_CR1_ARPE;	// count down, one-pulse mode, enable preload
	TIM3->CR2 = 0;
	TIM3->EGR = 0;
	TIM3->ARR = delay;
	TIM3->PSC = TIM_PRESCALER - 1;
	TIM3->CR1 |= TIM_CR1_CEN;			// enable   
}

// Bug: doesn't start with red -- why not?
void Task_RGB_FSM_Timer(void) {
	enum State { ST_RED, ST_RED_WAIT, ST_GREEN, ST_GREEN_WAIT, ST_BLUE,
		ST_BLUE_WAIT
	};
	static enum State next_state;

	if (g_flash_LED == 0) {				// Only run task when NOT in flash mode
		if (g_flash_LED_changed) {
			next_state = ST_RED;			// Restart with red
			g_flash_LED_changed = 0;
		}
		switch (next_state) {
		case ST_RED:
			Control_RGB_LEDs(1, 0, 0);
			Start_TIM(g_RGB_delay);
			next_state = ST_RED_WAIT;
			break;
		case ST_RED_WAIT:
			if (TIM_Expired()) {
				Stop_TIM();
				next_state = ST_GREEN;
			}
			break;
		case ST_GREEN:
			Control_RGB_LEDs(0, 1, 0);
			Start_TIM(g_RGB_delay);
			next_state = ST_GREEN_WAIT;
			break;
		case ST_GREEN_WAIT:
			if (TIM_Expired()) {
				Stop_TIM();
				next_state = ST_BLUE;
			}
			break;
		case ST_BLUE:
			Control_RGB_LEDs(0, 0, 1);
			Start_TIM(g_RGB_delay);
			next_state = ST_BLUE_WAIT;
			break;
		case ST_BLUE_WAIT:
			if (TIM_Expired()) {
				Stop_TIM();
				next_state = ST_RED;
			}
			break;
		default:
			next_state = ST_RED;
			break;
		}
	}
}

// Section extracted from Task_Flash_FSM_Timer below
//		if (g_flash_LED_changed) {
//			next_state = ST_WHITE;		// Restart with white
//			g_flash_LED_changed = 0;
//		}


// Start Listing 3.14
void Task_Flash_FSM_Timer(void) {
	enum State { ST_WHITE, ST_WHITE_WAIT, ST_BLACK, ST_BLACK_WAIT };
	static enum State next_state = ST_WHITE;

	if (g_flash_LED == 1) {				// Only run task when in flash mode
		switch (next_state) {
		case ST_WHITE:
			Control_RGB_LEDs(1, 1, 1);
			Start_TIM(g_w_delay);
			next_state = ST_WHITE_WAIT;
			break;
		case ST_WHITE_WAIT:
			if (TIM_Expired()) {
				Stop_TIM();
				next_state = ST_BLACK;
			}
			break;
		case ST_BLACK:
			Control_RGB_LEDs(0, 0, 0);
			Start_TIM(g_w_delay);
			next_state = ST_BLACK_WAIT;
			break;
		case ST_BLACK_WAIT:
			if (TIM_Expired()) {
				Stop_TIM();
				next_state = ST_WHITE;
			}
			break;
		default:
			next_state = ST_WHITE;
			break;
		}
	} else {
		next_state = ST_WHITE;
	}
}
// End Listing 3.14

// Start Listing 3.15 
void Flasher(void) {
	Init_GPIO_RGB();
	Init_GPIO_Switches_Interrupts();
	while (1) {
		Task_Flash_FSM_Timer();
		Task_RGB_FSM_Timer();
	}
}
// End Listing 3.15
int main(void) {
	Flasher();
}
