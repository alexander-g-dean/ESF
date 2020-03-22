/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>

#include "timers.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/

int main (void) {
	uint16_t i=0;
	volatile int32_t delay;
	Init_Blue_LED_PWM(PWM_PERIOD);

	// Flash forever
	while (1) {
			for (i=0; i<PWM_PERIOD; i++) {
				TPM0->CONTROLS[1].CnV = i;
				for (delay=0; delay<100; delay++)
					;
			}		
			for (i=PWM_PERIOD-1; i>0; i--) {
				TPM0->CONTROLS[1].CnV = i;
				for (delay=0; delay<100; delay++)
					;
			}		
	}
}
