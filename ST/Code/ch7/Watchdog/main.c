#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "rgb.h"
#include "clock.h"
#include "delay.h"
#include "switch.h"


WWDG_HandleTypeDef WWDG_Handle;
WWDG_InitTypeDef WWDG_Init;

// Start Listing Init_Service_WWDG
#define WWDG_REFRESH (_VAL2FLD(WWDG_CR_T, 0x7f))
#define WWDG_WINDOW (_VAL2FLD(WWDG_CFR_W, 0x7f))

void Init_WWDG(void) {
	// Enable peripheral clock for WWDG
	RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;
	// Select the bus clock divided by 4096 * 8
	WWDG->CFR =
			_VAL2FLD(WWDG_CFR_WDGTB, 3) | WWDG_WINDOW;
	// Load with initial counter value
	WWDG->CR = WWDG_REFRESH;
	// Activate the WWDG
	WWDG->CR |= WWDG_CR_WDGA;
}

void Service_WWDG(void) {
	WWDG->CR = WWDG_REFRESH;
}
// End Listing Service_WWDG

/* Listing 7.4 */
// Start Listing Flash_Reset_Cause
#define NUM_STARTUP_FLASHES (5)
#define STARTUP_FLASH_DURATION (50)

void Flash_Reset_Cause() {
	unsigned int n;
	for (n = 0; n < NUM_STARTUP_FLASHES; n++) {
		if (RCC->CSR & RCC_CSR_WWDGRSTF) {
			// Red: WWDG caused reset
			Control_RGB_LEDs(1, 0, 0);
		} else {
			// Green: WWDG did not cause reset
			Control_RGB_LEDs(0, 1, 0);
		}
		Delay(STARTUP_FLASH_DURATION);
		Control_RGB_LEDs(0, 0, 0);
		Delay(10 * STARTUP_FLASH_DURATION);
	}
	// Clear all reset cause flags
	RCC->CSR |= RCC_CSR_RMVF;
}
// End Listing Flash_Reset_Cause

/* Listing 7.5 */
// Start Listing WWDG_main
int main(void) {
	Set_Clocks_To_48MHz();
	Init_GPIO_RGB();
	Init_GPIO_Switches();
	// Show why system is starting up by flashing LEDs
	Flash_Reset_Cause();
	// Start up watchdog
	Init_WWDG();
	// Main loop: do or don't refresh watchdog
	while (1) {
		if (SWITCH_PRESSED(SW1_POS)) {
			// Don't service watchdog
			// Turn off LEDs - warning!
			Control_RGB_LEDs(0, 0, 0);
		} else {
			// Service watchdog
			Service_WWDG();
			// Light blue LED - OK!
			Control_RGB_LEDs(0, 0, 1);
		}
	}
}
// End Listing WWDG_main
