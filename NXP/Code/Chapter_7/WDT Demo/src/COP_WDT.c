#include <MKL25Z4.H>
#include "config.h"
#include "LEDs.h"
#include "delay.h"

void Init_COP_WDT(void) {
	// Select 1 kHz clock and 1024 cycle time-out
	SIM->COPC = SIM_COPC_COPT(3) & ~SIM_COPC_COPCLKS_MASK & ~SIM_COPC_COPW_MASK; 
	}

void Service_COP_WDT(void) {
	SIM->SRVCOP = 0x55;
	SIM->SRVCOP = 0xaa;
}

void Flash_Reset_Cause(void) {
	unsigned n;
	
	for (n=0; n<NUM_STARTUP_FLASHES; n++) {
		if (RCM->SRS0 & RCM_SRS0_WDOG_MASK) 
			Control_RGB_LEDs(1, 0, 0); // Red: WDOG caused reset
		else 
			Control_RGB_LEDs(0, 1, 0); // Green: WDOG did not cause reset
		
		Delay(STARTUP_FLASH_DURATION);
		Control_RGB_LEDs(0, 0, 0);
		Delay(2*STARTUP_FLASH_DURATION);
	}
}
