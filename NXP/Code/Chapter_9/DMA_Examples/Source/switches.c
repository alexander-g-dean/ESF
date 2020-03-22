#include <MKL25Z4.H>
#include "switches.h"
#include "LEDs.h"

volatile unsigned count=0;

void Init_Switch(void) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

	/* Select GPIO and enable pull-up resistors and no interrupts 
		for pins connected to switches */
	PORTD->PCR[SW_POS] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[SW_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
	
	/* Set port D switch bit to input */
	PTD->PDDR &= ~MASK(SW_POS);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
