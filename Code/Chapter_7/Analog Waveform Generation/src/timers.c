#include "timers.h"
#include "MKL25Z4.h"
#include "LEDs.h"
#include "user_defs.h"

void Init_TPM(void)
{
	// Turn on clock to TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	// Set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
	// Load the counter and mod, given prescaler of 32
	TPM0->MOD = (F_TPM_CLOCK/(F_TPM_OVFLW*32))-1;
	// Set TPM to divide by 32 prescaler, enable counting (CMOD) and interrupts
	TPM0->SC = TPM_SC_CMOD(1) | TPM_SC_PS(5) | TPM_SC_TOIE_MASK;
	// Enable interrupts in NVIC
	NVIC_SetPriority(TPM0_IRQn, 3); 
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);	

	// Note: TPM clock input is selected based on SIM SOPT2 register
	// TPMSRC = 01 (MCGFLLCLK or MCGPLLCLK/2)
	// PLLFLLSEL = 1 (MCGPLLCLK/2)
	// Use CLOCK_SETUP 1 in system_MKL25Z4.h
}

void TPM0_IRQHandler() {
	static int change=STEP_SIZE;
	static uint16_t out_data=0;

	FPTD->PSOR = 1 << BLUE_LED_POS; // Debug signal: Entering ISR
	TPM0->SC |= TPM_SC_TOIE_MASK; // reset overflow flag

	// Do ISR work 
	out_data += change;
	if (out_data < STEP_SIZE) {
			change = STEP_SIZE;
	} else if (out_data >= DAC_RESOLUTION-STEP_SIZE) {
			change = -STEP_SIZE;
	}
	DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
	DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);	
	FPTD->PCOR = 1 << BLUE_LED_POS;	// Debug signal: Exiting ISR
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
