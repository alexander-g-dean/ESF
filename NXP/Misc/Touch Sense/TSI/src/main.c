
//************************************************ Pre-Processor Directives *****************************************
#include <MKL25Z4.H>
#include <stdio.h>
#include "UART.h"
#include "gpio_defs.h"
#include "LEDs.h"
#include "delay.h"
#include "HBLED.h"
#include "tpm.h"

#define BUSY_WAIT 1 // implements the code in busy-wait loop
#define INTERRUPT 0 // implements the code in end of scan interrupt 

//variation of the capacitance from 90 to 700
#define TOUCH_OFFSET 550  // offset value to be subtracted
#define TOUCH_DATA (TSI0->DATA & 0xFFFF)//macro for extracting the count from data register

//************************************************Global Variables*********************************************
volatile uint32_t touch_val = 0 ;

typedef struct {	// Entry to translate from touch value range to color
	uint32_t max_touch_val;
	uint8_t R, G, B;
} TOUCH_RGB_ENTRY;

TOUCH_RGB_ENTRY RGB_Table[] = { // Table of translation entries
	{65,0,0,0}, // first recognized touch range
	{300,0,0,1},
	{500,0,1,1},
	{700,0,1,0},
	{900,1,1,0},
	{1200,1,0,0},
	{2000,1,0,1},
	{5000,1,1,1}, // last recognized touch range
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


//***************************************************Main Loop****************************************************
int main (void) {
	
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

		for (int i=0; RGB_Table[i].max_touch_val>0; i++) {
			if (touch_val < RGB_Table[i].max_touch_val) {
				Control_RGB_LEDs(RGB_Table[i].R, RGB_Table[i].G, RGB_Table[i].B);
				break;
			}
			Control_RGB_LEDs(0,0,0);
		}
		SysTick_Wait1ms(100);
		printf("Value of Touch Scan: %d \n\r",touch_val);	
	}
}
