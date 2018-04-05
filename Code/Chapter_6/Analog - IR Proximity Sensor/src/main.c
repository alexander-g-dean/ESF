#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"

int Threshold[NUM_RANGE_STEPS] = {34000, 27000, 20000, 14000, 8000, 0};

const int Colors[NUM_RANGE_STEPS][3] = {{ 1, 1, 1}, // white
																	{ 1, 0, 1}, // magenta
																	{ 1, 0, 0}, // red
																	{ 1, 1, 0}, // yellow
																	{ 0, 0, 1}, // blue
																	{ 0, 1, 0}  // green
};
																	 

void Init_ADC(void) {
	
	SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT); 
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLSMP_MASK |
		ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0); // 16 bit conversion mode

	ADC0->SC2 = ADC_SC2_REFSEL(0); // Select default voltage reference pins
}

void Control_IR_LED(unsigned int led_on) {
	if (led_on) {
			PTB->PCOR = MASK(IR_LED_POS);
	} else {
			PTB->PSOR = MASK(IR_LED_POS); 
	}
}	
void Init_IR_LED(void) {
	PORTB->PCR[IR_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[IR_LED_POS] |= PORT_PCR_MUX(1);          
	PTB->PDDR |= MASK(IR_LED_POS);
	
	// Start off with IR LED turned off
	Control_IR_LED(0);
}

unsigned Measure_IR(void) {
	volatile unsigned res=0;
	
	ADC0->SC1[0] = IR_PHOTOTRANSISTOR_CHANNEL; // start conversion on channel 0
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		; // Wait until conversion is complete
	res = ADC0->R[0];
	// Complement result since voltage falls with increasing IR level
	// but we want result to rise with increasing IR level
	return 0xffff-res;
}

void Display_Range(int b) {
	unsigned i;
	
	for (i=0; i<NUM_RANGE_STEPS-1; i++) {
		if (b > Threshold[i])
			break;
	}
	Control_RGB_LEDs(Colors[i][RED], Colors[i][GREEN], Colors[i][BLUE]);
}

void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	static int on_brightness=0, off_brightness=0;
	static int avg_diff;
	static int diff;
	unsigned n;
	
	Init_ADC();
	Init_RGB_LEDs();
	Init_IR_LED();
	Control_RGB_LEDs(0, 0, 0);
	
	while (1) {
		diff = 0;
		for (n=0; n<NUM_SAMPLES_TO_AVG; n++) {
			// Measure IR level with IRLED off
			Control_IR_LED(0);
			Delay_us(T_DELAY_OFF);
			off_brightness = Measure_IR();
		
			// Measure IR level with IRLED on
			Control_IR_LED(1);
			Delay_us(T_DELAY_ON);
			on_brightness = Measure_IR();

			// Calculate difference
			diff += on_brightness - off_brightness;
		}
		// Calculate average difference
		avg_diff = diff/NUM_SAMPLES_TO_AVG;
		// light RGB LED according to range
		Display_Range(avg_diff);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
