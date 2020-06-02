#include <stm32f091xc.h>
#include "rgb.h"
#include "clock.h"

/* Listing 7.1 */
// Start Listing Init_SysTick
#define F_SYS_CLK (48000000L)
void Init_SysTick(void) {
	// SysTick is defined in core_cm0.h
	// Set reload to get 1s interrupts
	SysTick->LOAD = (F_SYS_CLK / 8) - 1;

	// Set interrupt priority
	NVIC_SetPriority(SysTick_IRQn, 3);
	// Force load of reload value
	SysTick->VAL = 0;
	// Enable interrupt, enable SysTick timer
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk |
			SysTick_CTRL_ENABLE_Msk;
}

// End Listing Init_SysTick

/* Listing 7.2 */
// Start Listing SysTick_Handler
void SysTick_Handler(void) {
	static int n = 0;
	Control_RGB_LEDs(n & 1, n & 1, n & 1);
	n++;
}

// End Listing SysTick_Handler

extern void PLL_Modification(void);

// Start Listing SysTick_main
int main(void) {
	Set_Clocks_To_48MHz();
	Init_GPIO_RGB();
	Init_SysTick();
	SystemCoreClockUpdate();
	while (1)
		;
}
// End Listing SysTick_main
