#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"

void Init_Comparator(void) {
	// Enable clock to comparator
	SIM->SCGC4 |= SIM_SCGC4_CMP_MASK; 
	
	// Enable Comparator
	CMP0->CR1 = CMP_CR1_EN_MASK;

	// Select input channels
	// Plus: channel 5 on Port E bit 29
	// Minus: CMP DAC is channel 7
	CMP0->MUXCR = CMP_MUXCR_PSEL(5) | CMP_MUXCR_MSEL(7);

	// Enable DAC, set reference voltage at 32/64 = 1/2 VREFH
	CMP0->DACCR = CMP_DACCR_DACEN_MASK | CMP_DACCR_VOSEL(32);
		
	// Enable interrupt for Comparator on both edges
	CMP0->SCR = CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK;

	NVIC_SetPriority(CMP0_IRQn, 128);
	NVIC_ClearPendingIRQ(CMP0_IRQn);
	NVIC_EnableIRQ(CMP0_IRQn);	
}

void CMP0_IRQHandler(void) {
	// set break point here to observe operation
	if (CMP0->SCR & CMP_SCR_CFR_MASK) { // rising 
		// light green LED
		Control_RGB_LEDs(0, 1, 0);
	} else if (CMP0->SCR & CMP_SCR_CFF_MASK) { // falling
		// light red LED
		Control_RGB_LEDs(1, 0, 0);
	}
	// clear flags, keep interrupt on both edges enabled
	CMP0->SCR = CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK |
							CMP_SCR_CFR_MASK | CMP_SCR_CFF_MASK;
}

void Test_Comparator(void) {
	unsigned n;

	Init_Comparator();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,0);

#if 1
	// Interrupt version: Red and Green
	__enable_irq();
	while (1) 
			;
#else	
	while (1) {
	// Polled version: Yellow and Blue
		n = (CMP0->SCR & CMP_SCR_COUT_MASK);
		if (n)
			Control_RGB_LEDs(1, 1, 0); // Yellow
		else
			Control_RGB_LEDs(0, 0, 1); // Blue
	}
#endif
}

#define ADC_POS (20)

void Init_ADC(void) {
	
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 
	
	// Select analog for pin
	PORTE->PCR[ADC_POS] &= ~PORT_PCR_MUX_MASK;	
	PORTE->PCR[ADC_POS] |= PORT_PCR_MUX(0);	
	
	// Low power configuration, long sample time, 16 bit single-ended conversion, bus clock input
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0);
	// Software trigger, compare function disabled, DMA disabled, voltage references VREFH and VREFL
	ADC0->SC2 = ADC_SC2_REFSEL(0);
}

#define SCALE_FACTOR (3.2/2.2)

float Measure_VBat(void) {
	volatile float vref, vbat;
	volatile unsigned res=0;
	volatile unsigned sum = 0;
	
	vref = 2.92; // measured from power supply rail
	
	ADC0->SC1[0] = 0x00; // start conversion on channel 0
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;
	res = ADC0->R[0];
	vbat = SCALE_FACTOR*vref*res/0xffff;
	return vbat;
}

#define DAC_POS (30)

// Listing 6.1
void Init_DAC(void) {
  // Enable clock to DAC and Port E
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Select analog for pin
	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[DAC_POS] |= PORT_PCR_MUX(0);	
		
	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;
	
	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}

// Listing 6.2
void Triangle_Output(void) {
	int i=0, change=1;
	
	while (1) {
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(i);
		DAC0->DAT[0].DATH = DAC_DATH_DATA1(i >> 8);

		i += change;
		if (i ==0) {
			change = 1;
		} else if (i == DAC_RESOLUTION-1) {
			change = -1;
		}	
	}
}

void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}

/*------------------------------------------------------------------------------
	Code for driving speaker
	period: microseconds
	duration: milliseconds
*------------------------------------------------------------------------------*/
void Play_Tone(unsigned int period, unsigned int duration, unsigned wave_type) {
	unsigned num_cycles = duration*1000/period;
	unsigned step, out_data;
	
	while (num_cycles--) {
		for (step = 0; step < NUM_STEPS; step++) {
			switch (wave_type) {
				case SQUARE: 
					if (step < NUM_STEPS/2)
						out_data = 1;	
					else
						out_data = 0;
					break;
				case RAMP:
					out_data = (step*DAC_RESOLUTION)/NUM_STEPS;
					break;
			default:
					break;
		}
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);
		DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
		Delay_us(period/NUM_STEPS);
		}
	}
}


float Measure_Temperature(void){
	float n, temp;
	
	ADC0->SC1[0] = 0x00; // start conversion on channel 0
	
	// Wait for conversion to finish
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		;
	// Read result, convert to floating-point
	n = (float) ADC0->R[0];
	
	// Calculate temperature (Celsius) using polynomial equation
	// Assumes ADC is in 16-bit mode, has VRef = 3.3 V

	temp = -36.9861 + n*(0.0155762 + n*(-1.43216E-06 + n*(7.18641E-11 
		+ n*(-1.84630E-15 + n*(2.32656E-20 + n*(-1.13090E-25))))));
	return temp;
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
#if 0
	volatile float t=0.0;
	
	Init_ADC();
	while (1) {
		t = Measure_Temperature();
	}
#endif
	
#if 0
	Test_Comparator();
#endif
	
#if 0	
	volatile float vb;
	
	Init_ADC();
	while (1) {
		vb = Measure_VBat();
	}
#endif

#if 1	
	Init_DAC();
	Triangle_Output();
#endif

#if 0
	Init_DAC();
	while (1) {
		Play_Tone(1000, 500, RAMP);
		Play_Tone(1000, 500, SQUARE);
	}
#endif
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
