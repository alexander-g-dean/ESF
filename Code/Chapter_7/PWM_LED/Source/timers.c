#include "timers.h"
#include <MKL25Z4.h> 
#include "LEDs.h"

void Init_Blue_LED_PWM(uint16_t period){
	// Enable clock to port D
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;;
	
	// Blue FTM0_CH1, Mux Alt 4
	// Set pin to FTMs
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(4);          

	// Configure TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	//set clock source for tpm: 48 MHz
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	//load the counter and mod
	TPM0->MOD = period-1;  
	//set TPM count direction to up with a divide by 2 prescaler 
	TPM0->SC =  TPM_SC_PS(1);
	// Continue operation in debug mode
	TPM0->CONF |= TPM_CONF_DBGMODE(3);
	// Set channel 1 to edge-aligned low-true PWM
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
	// Set initial duty cycle
	TPM0->CONTROLS[1].CnV = 0;
	// Start TPM
	TPM0->SC |= TPM_SC_CMOD(1);
}


// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
