// Start Listing 3.1
#include <stm32f091xc.h>
#include "switch.h"
#include "rgb.h"
#include "delay.h"

#define W_DELAY_SLOW 	 150
#define W_DELAY_FAST 		70
#define RGB_DELAY_SLOW 400
#define RGB_DELAY_FAST 100

void Flasher(void) {
	uint32_t w_delay = W_DELAY_SLOW;
	uint32_t RGB_delay = RGB_DELAY_SLOW;

	Init_GPIO_RGB();
	Init_GPIO_Switches();
	while (1) {
		if (SWITCH_PRESSED(SW1_POS)) {	// flash white
			Control_RGB_LEDs(1, 1, 1);
			Delay(w_delay);
			Control_RGB_LEDs(0, 0, 0);
			Delay(w_delay);
		} else {										// sequence R, G, B
			Control_RGB_LEDs(1, 0, 0);
			Delay(RGB_delay);
			Control_RGB_LEDs(0, 1, 0);
			Delay(RGB_delay);
			Control_RGB_LEDs(0, 0, 1);
			Delay(RGB_delay);
		}
		if (SWITCH_PRESSED(SW2_POS)) {
			w_delay = W_DELAY_FAST;
			RGB_delay = RGB_DELAY_FAST;
		} else {
			w_delay = W_DELAY_SLOW;
			RGB_delay = RGB_DELAY_SLOW;
		}
	}
}
// End Listing 3.1

int main(void) {
	Flasher();
}
