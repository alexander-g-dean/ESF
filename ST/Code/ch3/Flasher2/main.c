#include <stm32f091xc.h>
#include "delay.h"
#include "switch.h"
#include "rgb.h"

// Start Listing 3.3
#define W_DELAY_SLOW 	 150
#define W_DELAY_FAST 		70
#define RGB_DELAY_SLOW 400
#define RGB_DELAY_FAST 100

// Start Listing 3.3
volatile uint8_t g_flash_LED = 0;
volatile uint32_t g_w_delay = W_DELAY_SLOW;
volatile uint32_t g_RGB_delay = RGB_DELAY_SLOW;

void Task_Read_Switches(void) {
    if (SWITCH_PRESSED(SW1_POS)) {
        g_flash_LED = 1; // flash white
    } else {
        g_flash_LED = 0; // RGB sequence
    }
    if (SWITCH_PRESSED(SW2_POS)) { 
        g_w_delay = W_DELAY_FAST;
        g_RGB_delay = RGB_DELAY_FAST;
    } else {
        g_w_delay = W_DELAY_SLOW;
        g_RGB_delay = RGB_DELAY_SLOW;
    }
}
// End Listing 3.3

// Start Listing 3.4
void Task_Flash(void) {
    if (g_flash_LED == 1) { // Only run task when in flash mode
        Control_RGB_LEDs(1, 1, 1);
        Delay(g_w_delay);
        Control_RGB_LEDs(0, 0, 0);
        Delay(g_w_delay);
    }
}
// End Listing 3.4

// Start Listing 3.5
void Task_RGB(void) {
    if (g_flash_LED == 0) { //only run task when NOT in flash mode
        Control_RGB_LEDs(1, 0, 0);
        Delay(g_RGB_delay);
        Control_RGB_LEDs(0, 1, 0);
        Delay(g_RGB_delay);
        Control_RGB_LEDs(0, 0, 1);
        Delay(g_RGB_delay);
    }
}
// End Listing 3.5

// Start Listing 3.6
void Flasher(void) {
    Init_GPIO_RGB();
		Init_GPIO_Switches();
    while (1) {
        Task_Read_Switches();
        Task_Flash();
        Task_RGB();
    }
}
// End Listing 3.6

int main(void) {
    Flasher();
}
