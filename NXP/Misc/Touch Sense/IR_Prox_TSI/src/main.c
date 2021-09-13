
//************************************************ Pre-Processor Directives *****************************************
#include <MKL25Z4.H>
#include <stdio.h>
#include "UART.h"
#include "gpio_defs.h"
#include "LEDs.h"
#include "delay.h"
#include "HBLED.h"
#include "tpm.h"
#include "user_defs.h"

#define BUSY_WAIT 1 // implements the code in busy-wait loop
#define INTERRUPT 0 // implements the code in end of scan interrupt 

//variation of the capacitance from 90 to 700
#define TOUCH_OFFSET 550  // offset value to be subtracted
#define TOUCH_DATA (TSI0->DATA & 0xFFFF)//macro for extracting the count from data register

//************************************************Global Variables*********************************************
volatile uint32_t touch_val = 0 ;

typedef struct {	// Entry to translate from touch value range to color
	uint32_t max_val;
	uint8_t R, G, B;
} SENSE_RGB_ENTRY;

SENSE_RGB_ENTRY Touch_RGB_Table[] = { // Table of translation entries
	{65,0,0,0}, // first recognized touch range
	{300,0,0,1}, 	// blue
	{500,0,1,1},  // cyan
	{700,0,1,0},	// green
	{900,1,1,0},	// yellow
	{1200,1,0,0},	// red
	{2000,1,0,1},	// magenta
	{5000,1,1,1}, // white, last recognized touch range
	{0, 0, 0}			// Sentinel to end table/loop
};

// IR Proximity sensor
volatile int avg_diff=0, diff=0;

SENSE_RGB_ENTRY IR_RGB_Table[] = { // Table of translation entries
	{100,0,0,0}, // first recognized IR range
	{300,0,0,1}, 	// blue
	{600,0,1,1},  // cyan
	{1200,0,1,0},	// green
	{2400,1,1,0},	// yellow
	{4000,1,0,0},	// red
	{6000,1,0,1},	// magenta
	{32000,1,1,1}, // white, last recognized IR range
	{0, 0, 0}			// Sentinel to end table/loop
};

//************************************************Function Prototypes*********************************************
#if INTERRUPT
void TSI0_IRQHandler (void){
	touch_val = TOUCH_DATA - TOUCH_OFFSET;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; 	//writing one to clear the flag
}
#endif

void Touch_Init()
{
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK; // enabling the clock
	
#if BUSY_WAIT
	TSI0->GENCS = TSI_GENCS_MODE(0u) | //operating in non-noise mode
								TSI_GENCS_REFCHRG(0u) | //reference oscillator charge and discharge value 500nA
								TSI_GENCS_DVOLT(0u) | //oscillator voltage rails set to default 
								TSI_GENCS_EXTCHRG(0u) | //electrode oscillator charge and discharge value 500nA
								TSI_GENCS_PS(0u) |  // frequency clcok divided by one
								TSI_GENCS_NSCN(31u) | //scanning the electrode 32 times
								TSI_GENCS_TSIEN_MASK | //enabling the TSI module
								TSI_GENCS_EOSF_MASK; // writing one to clear the end of scan flag
#endif	
	
#if INTERRUPT	
	TSI0->GENCS = TSI_GENCS_MODE(0u) |
								TSI_GENCS_REFCHRG(0u) | 
								TSI_GENCS_DVOLT(0u) | 
								TSI_GENCS_EXTCHRG(0u) | 
								TSI_GENCS_PS(0u) | 
								TSI_GENCS_NSCN(31u) | 
								TSI_GENCS_TSIEN_MASK |
								TSI_GENCS_STPE_MASK |  // enabling the TSI in low power modes
								TSI_GENCS_EOSF_MASK |
								TSI_GENCS_ESOR_MASK | //enabling interrupt using end of scan
								TSI_GENCS_TSIIEN_MASK; //enabling the TSI interrupt

	TSI0->DATA = 	TSI_DATA_TSICH(10u); // selecting channel 10
	//enaling interrupt in NVIC
	NVIC_SetPriority(TSI0_IRQn, 2); 
	NVIC_ClearPendingIRQ(TSI0_IRQn); 
	NVIC_EnableIRQ(TSI0_IRQn);	
	// Allow low leakage stop mode
	SMC->PMPROT = SMC_PMPROT_ALLS_MASK; // 
	// Enable low-leakage stop mode and regular run mode
	SMC->PMCTRL = SMC_PMCTRL_STOPM(3) | SMC_PMCTRL_RUNM(0);
	SMC->STOPCTRL = SMC_STOPCTRL_PSTOPO(0) | SMC_STOPCTRL_VLLSM(3);
	// Enable LLWU
	// allow TSI0 to wake LLWU
	LLWU->ME |= LLWU_ME_WUME4_MASK;
#endif				
}

#if BUSY_WAIT
int Touch_Scan_LH(void)
{
	unsigned int scan = 0;
	TSI0->DATA = 	TSI_DATA_TSICH(10u); 
	TSI0->DATA |= TSI_DATA_SWTS_MASK; //software trigger to start the scan
	while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK )) // waiting for the scan to complete 32 times
	;	
	scan = TOUCH_DATA;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; //writing one to clear the end of scan flag
	return scan- TOUCH_OFFSET;
}
#endif

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
	
	ADC0->SC1[0] = IR_PHOTOTRANSISTOR_CHANNEL; // start conversion on IR channel
	
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
		; // Wait until conversion is complete
	res = ADC0->R[0];
	// Complement result since voltage falls with increasing IR level
	// but we want result to rise with increasing IR level
	return 0xffff-res;
}

void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}

void Scale_Values(SENSE_RGB_ENTRY table[], float factor) {
	for (int i=0; table[i].max_val>0; i++) {
		table[i].max_val *= factor;
	}
}

//***************************************************Main Loop****************************************************

int main (void) {
	static int on_brightness=0, off_brightness=0;
	int n;
	
	Init_UART0(115200); 
	SysTick_Init();
	Init_RGB_LEDs();
	Touch_Init();
	Init_ADC();
	Init_IR_LED();

	printf("Touch Sense Demo\n\r"); //using UART to print the touch scan value
	
	while (1) {
		
#if 1 // Show IR range
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
		for (int i=0; IR_RGB_Table[i].max_val>0; i++) {
			if (avg_diff < IR_RGB_Table[i].max_val) {
				Control_RGB_LEDs(IR_RGB_Table[i].R, IR_RGB_Table[i].G, IR_RGB_Table[i].B);
				break;
			}
			Control_RGB_LEDs(0,0,0);
		}
#endif
#if 1 // Touch sense
		#if BUSY_WAIT
		touch_val = Touch_Scan_LH();
		#endif
		
		#if INTERRUPT
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		__wfi() ; // go to sleep	
		#endif

		if ((touch_val > 250) && 
			(touch_val < 1500)) {
				if (touch_val < 800) {
					Control_RGB_LEDs(1,0,0);
					Scale_Values(IR_RGB_Table, 1.02);
				} else {
					Control_RGB_LEDs(0,1,0);
					Scale_Values(IR_RGB_Table, 0.98);
				}
				SysTick_Wait1ms(100);
				Control_RGB_LEDs(0,0,0);
		}
	#if 0
		for (int i=0; Touch_RGB_Table[i].max_val>0; i++) {
			if (touch_val < Touch_RGB_Table[i].max_val) {
				Control_RGB_LEDs(Touch_RGB_Table[i].R, Touch_RGB_Table[i].G, Touch_RGB_Table[i].B);
				break;
			}
			Control_RGB_LEDs(0,0,0);
		}
		SysTick_Wait1ms(100);
	#endif
#endif	
		printf("Value of Touch Scan: %d \n\r",touch_val);	
	}
}

int TSI_main (void) {
	
	Init_UART0(115200); 
	SysTick_Init();
	Init_RGB_LEDs();
	Touch_Init();

	printf("Touch Sense Demo\n\r"); //using UART to print the touch scan value
	
	while (1) {
		
		#if BUSY_WAIT
		touch_val = Touch_Scan_LH();
		#endif
		
		#if INTERRUPT
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		__wfi() ; // go to sleep	
		#endif

		for (int i=0; Touch_RGB_Table[i].max_val>0; i++) {
			if (touch_val < Touch_RGB_Table[i].max_val) {
				Control_RGB_LEDs(Touch_RGB_Table[i].R, Touch_RGB_Table[i].G, Touch_RGB_Table[i].B);
				break;
			}
			Control_RGB_LEDs(0,0,0);
		}
		SysTick_Wait1ms(100);
		printf("Value of Touch Scan: %d \n\r",touch_val);	
	}
}
