#include <stm32f091xc.h>
#include "delay.h"
#include "rgb.h"

// Start Listing IR_Prox_Init_ADC
void Init_ADC(void) {
	// Enable peripheral clock of ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configure PA0 as analog input to ADC
	// Set both MODER bits
	GPIOA->MODER |= GPIO_MODER_MODER0;
	// Oscillator: Enable and Select HSI14 (from STM32F0 Reference Manual, A.7.4)
	RCC->CR2 |= RCC_CR2_HSI14ON;	 /* (2) Start HSI14 RC oscillator */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {	/* (3) Wait HSI14 is ready */
		/* For robust implementation, add here time-out management */
	}
	ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE);	/* (4) Select HSI14 with CKMODE=00 */
	// Init ADCl
	ADC1->SMPR &= ~ADC_SMPR_SMP;	// SMP = 000 for minimum sample time

	/* CFGR1: The default configuration (CFGR1 = 0) matches what we want: 
	some features are disabled (analog watchdog, discontinous conversion 
	mode, auto-off mode, wait conversion mode, continuous conversion mode, 
	hardware trigger) and other features are selected: software trigger, 
	right-aligned data, 12-bit resolution. */
	ADC1->CFGR1 = 0;
	
	ADC1->CHSELR = ADC_CHSELR_CHSEL0;		// Select ADC input channel 0

	// Enable ADC (from STM32F0 Reference Manual, A.7.2
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) {	/* (1) Ensure that ADRDY = 0 */
		ADC1->ISR |= ADC_ISR_ADRDY;	/* (2) Clear ADRDY */
	}
	ADC1->CR |= ADC_CR_ADEN;			/* (3) Enable the ADC */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {	/* (4) Wait until ADC ready */
		/* For robust implementation, add here time-out management */
	}
}
// End Listing IR_Prox_Init_ADC

// Start Listing IR_Prox_Init_IR
#define IR_LED_OFF_MSK (GPIO_BSRR_BS_0)
#define IR_LED_ON_MSK (GPIO_BSRR_BR_0)

void Init_IR_LED(void) {
	// Enable peripheral clock of GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Init PB0 as digital output
	// Configure the pin as output
	// Clear mode field  to 00
	GPIOB->MODER &= ~GPIO_MODER_MODER0;
	// Set mode field to 01 for output
	GPIOB->MODER |= _VAL2FLD(GPIO_MODER_MODER0, 1);
	// Turn off IR LED
	GPIOB->BSRR = IR_LED_OFF_MSK;
}
// End Listing IR_Prox_Init_IR

// Start Listing IR_Prox_Control_Measure
void Control_IR_LED(unsigned int led_on) {
	GPIOB->BSRR =
			led_on ? IR_LED_ON_MSK : IR_LED_OFF_MSK;
}

unsigned int Measure_IR(void) {
	unsigned int n;

	// Start the conversion
	ADC1->CR |= ADC_CR_ADSTART;
	// Busy wait until the conversion is done 
	while (!(ADC1->ISR & ADC_ISR_EOC))
		;
	// Read the ADC value
	n = ADC1->DR;
	// We want return number to rise with increasing
	// IR but voltage falls with increasing IR.
	// So subtract result from max possible value 
	// (0xfff for 12-bit conversion) 
	return 0xfff - n;
}
// End Listing IR_Prox_Control_Measure

// Start Listing IR_Prox_display
#define RED (0)
#define GREEN (1)
#define BLUE (2)
#define NUM_RANGE_STEPS (7)

const int Threshold[NUM_RANGE_STEPS] =
		{ 500, 200, 80, 30, 15, 10, 0 };

const int Colors[NUM_RANGE_STEPS][3] = {
	{1, 1, 1},			// white
	{1, 0, 1},			// magenta
	{1, 0, 0},			// red
	{1, 1, 0},			// yellow
	{0, 0, 1},			// blue
	{0, 1, 0},			// green
	{0, 0, 0}				// off
};

void Display_IR_Reflectance(int b) {
	unsigned int i;

	for (i = 0; i < NUM_RANGE_STEPS - 1; i++) {
		if (b > Threshold[i])
			break; // break out of for loop
	}
	Control_RGB_LEDs(Colors[i][RED],
									 Colors[i][GREEN],
									 Colors[i][BLUE]);
}
// End Listing IR_Prox_display

// Start Listing IR_Prox_main
#define T_DELAY_ON (400)
#define T_DELAY_OFF (400)
#define NUM_SAMPLES_TO_AVG (20)

int main(void) {
	/* Making local variables static makes 
	   them visible to debugger  */
	static int on_brightness=0, off_brightness=0;
	static int avg_diff, diff;
	unsigned int n;

	Init_ADC();
	Init_GPIO_RGB();
	Init_IR_LED();
	Control_RGB_LEDs(0, 0, 0);

	while (1) {
		diff = 0;
		for (n = 0; n < NUM_SAMPLES_TO_AVG; n++) {
			// Measure IR level with IRLED off
			Control_IR_LED(0);
			ShortDelay(T_DELAY_OFF);
			off_brightness = Measure_IR();
			// Measure IR level with IRLED on
			Control_IR_LED(1);
			ShortDelay(T_DELAY_ON);
			on_brightness = Measure_IR();
			// Compute difference
			diff += on_brightness - off_brightness;
		}
		// Compute average difference
		avg_diff = diff / NUM_SAMPLES_TO_AVG;
		// Light RGB LED according to reflectance
		Display_IR_Reflectance(avg_diff);
	}
}

// End Listing IR_Prox_main
