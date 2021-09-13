#include <MKL25Z4.h>
#include "tpm.h"
#include "HBLED.h"
#include "GPIO_defs.h"

void PWM_Init(TPM_Type * TPM, uint8_t channel_num, uint16_t period, uint16_t duty)
{
	
	// Configure PTE31 for TPM0 Ch 4
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[31]  &= PORT_PCR_MUX(7);
	PORTE->PCR[31]  |= PORT_PCR_MUX(3);
	
	//turn on clock to TPM 
	switch ((int) TPM) {
		case (int) TPM0:
			SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
			break;
		case (int) TPM1:
			SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
			break;
		case (int) TPM2:
			SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
			break;
		default:
			break;
	}
// Configure TPM
// Set clock source for tpm: 48 MHz
SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
// Load the counter and mod
TPM->MOD = period-1;
// Set TPM count direction to up with a divide by 32 prescaler
TPM->SC = TPM_SC_PS(5);
// Continue operation in debug mode
TPM->CONF |= TPM_CONF_DBGMODE(3);
// Set channel 1 to edge-aligned low-true PWM
TPM->CONTROLS[channel_num].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
// Set initial duty cycle
TPM->CONTROLS[channel_num].CnV = duty;
// Start TPM
TPM->SC |= TPM_SC_CMOD(1);
}

void PWM_Set_Value(TPM_Type * TPM, uint8_t channel_num, uint16_t value) {
	TPM->CONTROLS[channel_num].CnV = value;
}
