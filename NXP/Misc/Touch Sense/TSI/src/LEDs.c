#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"

void Init_RGB_LEDs(void) {
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	// Debug port Initialization
	PORTB->PCR[DEBUG0_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG0_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DEBUG1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG1_POS] |= PORT_PCR_MUX(1);  
	PORTB->PCR[DEBUG2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG2_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DEBUG3_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DEBUG3_POS] |= PORT_PCR_MUX(1);  
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);
	//Debug port direction
	PTB->PDDR |= MASK(DEBUG0_POS) | MASK(DEBUG1_POS)| MASK(DEBUG2_POS)| MASK(DEBUG3_POS);
}

void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	if (red_on) {
			PTB->PCOR = MASK(RED_LED_POS);
	} else {
			PTB->PSOR = MASK(RED_LED_POS); 
	}
	if (green_on) {
			PTB->PCOR = MASK(GREEN_LED_POS);
	}	else {
			PTB->PSOR = MASK(GREEN_LED_POS); 
	} 
	if (blue_on) {
			PTD->PCOR = MASK(BLUE_LED_POS);
	}	else {
			PTD->PSOR = MASK(BLUE_LED_POS); 
	}
}	
 
