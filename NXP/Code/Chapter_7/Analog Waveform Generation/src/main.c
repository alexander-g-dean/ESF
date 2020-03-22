#include <MKL25Z4.H>
#include <math.h>
#include "user_defs.h"
#include "LEDs.h"
#include "timers.h"

void Init_DAC(void) {
  // Init DAC output
	
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 
	
	PORTE->PCR[DAC_POS] &= ~(PORT_PCR_MUX(7));	// Select analog 
		
	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;
	
	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}

void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}

/*------------------------------------------------------------------------------
	Code for driving DAC
	period (of output waveform): microseconds
	duration: cycles (of output waveform)
*------------------------------------------------------------------------------*/
#if 0
void Play_Tone_with_Busy_Waiting(unsigned int period, unsigned int num_cycles, unsigned wave_type) {
	unsigned step, out_data;
	
	while (num_cycles>0) {
		num_cycles--;
		for (step = 0; step < NUM_STEPS; step++) {
			switch (wave_type) {
				case SQUARE: 
					if (step < NUM_STEPS/2)
						out_data = 0;	
					else
						out_data = MAX_DAC_CODE;
					break;
				case RAMP:
					out_data = (step*MAX_DAC_CODE)/NUM_STEPS;
					break;
				case SINE:
					out_data = SineTable[step];
					break;
			default:
					break;
			}

			while (!(PTD->PDIR & MASK(SW_POS)))
				;

			DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
			DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);
			Delay_us(period/NUM_STEPS);
		}
	}
}
#endif

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,1);
	Init_DAC();
	Init_TPM();
	while (1)
		;
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
