/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>
#include "gpio_defs.h"
#include "config.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "delay.h"
#include "COP_WDT.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_COP_WDT();
	Service_COP_WDT();
	Init_RGB_LEDs();
	Flash_Reset_Cause(); 				// Show system is starting up by flashing LEDs
	Service_COP_WDT();
	
	i2c_init();						// Init i2c	
	if (!init_mma()) {				// Init mma peripheral 
		Control_RGB_LEDs(1, 0, 1);	// Light purple error LED 
		while (1)					// Not able to initialize MMA
			;
	}
	Service_COP_WDT();
	Delay(300); 						// Delay before starting rest of program

	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		if ((fabs(roll) > MAX_ANGLE) | (fabs(pitch) > MAX_ANGLE)) {
			Control_RGB_LEDs(1, 1, 0); // Light yellow LED as warning
		} else {
			Control_RGB_LEDs(0, 1, 0); // Light green LED - OK!
			Service_COP_WDT();
		}
	}
}

