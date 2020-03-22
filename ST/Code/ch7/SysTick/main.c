#include <stm32f091xc.h>
#include "rgb.h"
#include "clock.h"

/* Listing 7.1 */
void Init_SysTick(void) {
    // Set reload to get 1s interrupts
    SysTick->LOAD = (48000000L / 8) - 1;
    // Set interrupt priority
    NVIC_SetPriority(SysTick_IRQn, 3);
    // Force load of reload value
    SysTick->VAL = 0;
    // Enable interrupt, enable SysTick timer
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

/* Listing 7.2 */
void SysTick_Handler(void) {
    static int n = 0;
    Control_RGB_LEDs(n&1, n&1, n&1);
    n++;
}

int main(void) {
	  Set_Clocks_To_48MHz();
    Init_GPIO_RGB();
	  Init_SysTick();
	  while (1);
}
