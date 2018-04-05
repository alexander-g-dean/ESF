#include <MKL25Z4.H>
#include "gpio_defs.h"

/*
	Code to demonstrate flashing RGB LEDs in sequence based on switch presses.
	Wiring Needed:
			Switch 1: From SW1_POS PTD7 to ground. Internal pull-up resistor.
			Switch 2: From SW2_POS PTD6	to ground. Internal pull-up resistor.
		
	Code phases:
			1: Base with busy waits. Listings 3.1, 3.2
			2: Tasks. Listings 3.3, 3.4, 3.5, 3.6
			3: Interrupt to read switches. Listings 3.7, 3.8
			4: FSM LED tasks, but no interrupt for switches. Listings 3.11, 3.12, 3.13
			5: Add HW Timer to FSM LED and switch tasks. Listing 3.14, 3.15
			6: Interrupt to read switches.
*/

void Delay(volatile unsigned int time_del) {
	volatile int n;
	while (time_del--) {
		n = 1000;
		while (n--)
			;
	}
}

void Initialize_Ports(void) {
	// Enable clock to ports A, B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set LED port bits to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);

	// Select port D on pin mux, enable pull-up resistors
	PORTD->PCR[SW1_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
	PORTD->PCR[SW2_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

	// Clear switch bits to input
	PTD->PDDR &= ~MASK(SW1_POS); 
	PTD->PDDR &= ~MASK(SW2_POS); 
	
	// Turn off LEDs
	PTB->PSOR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PSOR |= MASK(BLUE_LED_POS);
}

void Init_Debug_Signals(void) {
	// Enable clock to port B
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Make pins GPIO
	PORTB->PCR[DBG_1] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_1] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_2] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_2] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_3] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_3] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_4] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_4] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_5] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_5] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_6] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_6] |= PORT_PCR_MUX(1);          
	PORTB->PCR[DBG_7] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[DBG_7] |= PORT_PCR_MUX(1);          

	
	// Set ports to outputs
	PTB->PDDR |= MASK(DBG_1) | MASK(DBG_2) | MASK(DBG_3) | MASK(DBG_4) | MASK(DBG_5) | MASK(DBG_6) | MASK(DBG_7);
	
	// Initial values are 0
	PTB->PCOR = MASK(DBG_1) | MASK(DBG_2) | MASK(DBG_3) | MASK(DBG_4) | MASK(DBG_5) | MASK(DBG_6) | MASK(DBG_7);
}	

void Initialize_Interrupts(void) {
	/* Configure PORT peripheral. Select GPIO and enable pull-up 
	resistors and interrupts on all edges for pins connected to switches */
	PORTD->PCR[SW1_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | 
		PORT_PCR_PE_MASK | PORT_PCR_IRQC(11);
	PORTD->PCR[SW2_POS] = PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | 
		PORT_PCR_PE_MASK | PORT_PCR_IRQC(11);

	/* Configure NVIC */
	NVIC_SetPriority(PORTD_IRQn, 128);
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
	
	/* Configure PRIMASK */
	__enable_irq();
}
	
void Control_RGB_LEDs(int r_on, int g_on, int b_on) {
	if (r_on)
		PTB->PCOR = MASK(RED_LED_POS);
	else
		PTB->PSOR = MASK(RED_LED_POS);
	if (g_on)
		PTB->PCOR = MASK(GREEN_LED_POS);
	else
		PTB->PSOR = MASK(GREEN_LED_POS);
	if (b_on)
		PTD->PCOR = MASK(BLUE_LED_POS);
	else
		PTD->PSOR = MASK(BLUE_LED_POS);
}

void Control_Red_LED(int on) {
	if (on)
		PTB->PCOR = MASK(RED_LED_POS);
	else
		PTB->PSOR = MASK(RED_LED_POS);
}

void Control_Green_LED(int on) {
	if (on)
		PTB->PCOR = MASK(GREEN_LED_POS);
	else
		PTB->PSOR = MASK(GREEN_LED_POS);
}

void Control_Blue_LED(int on) {
	if (on)
		PTD->PCOR = MASK(BLUE_LED_POS);
	else
		PTD->PSOR = MASK(BLUE_LED_POS);
}

#if VER==1
// void Flasher_V1(void) {
	
void Flasher (void) {
	uint32_t w_delay=W_DELAY_SLOW;
	uint32_t RGB_delay=RGB_DELAY_SLOW;
	while (1) {
		if (SWITCH_PRESSED(SW1_POS)) { // flash white
			Control_RGB_LEDs(1, 1, 1);
			Delay(w_delay);
			Control_RGB_LEDs(0, 0, 0);
			Delay(w_delay);
		}	else { // sequence R, G, B
			Control_RGB_LEDs(1, 0, 0);
			Delay(RGB_delay);
			Control_RGB_LEDs(0, 1, 0);
			Delay(RGB_delay);
			Control_RGB_LEDs(0, 0, 1);
			Delay(RGB_delay);
			Control_RGB_LEDs(0, 0, 0);
		}
		if (SWITCH_PRESSED(SW2_POS)) {
			w_delay = W_DELAY_FAST;
			RGB_delay = RGB_DELAY_FAST;
		}	else {
			w_delay = W_DELAY_SLOW;
			RGB_delay = RGB_DELAY_SLOW;
		}
	}
}
#endif


#if (VER==2) | (VER==4)
// software delay
uint8_t g_flash_LED=0; 			// initially don't flash LED, just do RGB sequence
uint32_t g_w_delay=W_DELAY_SLOW; 		// delay for white flash
uint32_t g_RGB_delay=RGB_DELAY_SLOW; 	// delay for RGB sequence
#endif

// Variables shared with interrupts
#if (VER==3) | (VER==5) | (VER==6)
volatile uint8_t g_flash_LED=0; 			// initially don't flash LED, just do RGB sequence
volatile uint32_t g_w_delay=W_DELAY_SLOW; 		// delay for white flash
volatile uint32_t g_RGB_delay=RGB_DELAY_SLOW; 	// delay for RGB sequence
#endif

#if (VER==2) | (VER==3)
void Task_Flash(void) {
	PTB->PSOR = MASK(DBG_2);
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		Control_RGB_LEDs(1, 1, 1);
		Delay(g_w_delay);
		Control_RGB_LEDs(0, 0, 0);
		Delay(g_w_delay);
	}
	PTB->PCOR = MASK(DBG_2);
}

void Task_RGB(void) {
	PTB->PSOR = MASK(DBG_3);
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		Control_RGB_LEDs(1, 0, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 1, 0);
		Delay(g_RGB_delay);
		Control_RGB_LEDs(0, 0, 1);
		Delay(g_RGB_delay);
	}
	PTB->PCOR = MASK(DBG_3);

}
#endif

#if (VER==2) | (VER==4)
void Task_Read_Switches(void) {
	PTB->PSOR = MASK(DBG_1);
	if (SWITCH_PRESSED(SW1_POS)) { // flash white
		g_flash_LED = 1;
	} else {
		g_flash_LED = 0;
	}
	if (SWITCH_PRESSED(SW2_POS)) {
		g_w_delay = W_DELAY_FAST;
		g_RGB_delay = RGB_DELAY_FAST;
	}	else {
		g_w_delay = W_DELAY_SLOW;
		g_RGB_delay = RGB_DELAY_SLOW;
	}
	PTB->PCOR = MASK(DBG_1);
}
#endif

#if (VER==2)
// void Flasher_V2(void) {
void Flasher(void) {
	while (1) {
		Task_Read_Switches();
		Task_Flash();
		Task_RGB();
	}
}
#endif

#if (VER==3)
// void Flasher_V2_5(void) {
void Flasher(void) {
	while (1) {
		Task_Flash();
		Task_RGB();
	}
}
#endif

//////////////////////
#if (VER==4)
void Task_Flash_FSM(void) {
	static enum {ST_WHITE, ST_BLACK} next_state = ST_WHITE;

	PTB->PSOR = MASK(DBG_2);
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		switch (next_state) {
			case ST_WHITE:
				Control_RGB_LEDs(1, 1, 1);
				Delay(g_w_delay);
				next_state = ST_BLACK;
				break;
			case ST_BLACK:
				Control_RGB_LEDs(0, 0, 0);
				Delay(g_w_delay);
				next_state = ST_WHITE;
				break;
			default:
				next_state = ST_WHITE;
				break;
		}
	}
	PTB->PCOR = MASK(DBG_2);
}

void Task_RGB_FSM(void) {
	static enum {ST_RED, ST_GREEN, ST_BLUE, ST_OFF} next_state = ST_RED;

	PTB->PSOR = MASK(DBG_3);
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		switch (next_state) {
			case ST_RED:
				Control_RGB_LEDs(1, 0, 0);
				Delay(g_RGB_delay);
				next_state = ST_GREEN;
				break;
			case ST_GREEN:
				Control_RGB_LEDs(0, 1, 0);
				Delay(g_RGB_delay);
				next_state = ST_BLUE;
				break;
			case ST_BLUE:
				Control_RGB_LEDs(0, 0, 1);
				Delay(g_RGB_delay);
				next_state = ST_RED;
				break;
			default:
				next_state = ST_RED;
				break;
		}
	}
	PTB->PCOR = MASK(DBG_3);
}
// void Flasher_V3(void) {
void Flasher(void) {
	while (1) {
		Task_Read_Switches();
		Task_Flash_FSM();
		Task_RGB_FSM();
	}
}
#endif

// USE_IRQ
#if (VER==3) | (VER==6)
void PORTD_IRQHandler(void) {  
	PTB->PSOR = MASK(DBG_1);
	// Read switches
	if ((PORTD->ISFR & MASK(SW1_POS))) {	
		if (SWITCH_PRESSED(SW1_POS)) { // flash white
			g_flash_LED = 1;
		} else {
			g_flash_LED = 0;
		}
	}
	if ((PORTD->ISFR & MASK(SW2_POS))) {	
		if (SWITCH_PRESSED(SW2_POS)) {
			g_w_delay = W_DELAY_FAST;
			g_RGB_delay = RGB_DELAY_FAST;
		}	else {
			g_w_delay = W_DELAY_SLOW;
			g_RGB_delay = RGB_DELAY_SLOW;
		}
	}
	// clear status flags 
	PORTD->ISFR = 0xffffffff;
	PTB->PCOR = MASK(DBG_1);
}
#endif 

#if (VER==5) | (VER==6)
//////////////////////
// Use hardware timer
void Init_PIT(unsigned channel, unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Disable module
	PIT->MCR |= PIT_MCR_MDIS_MASK;
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	// Disable timer, clear control register
	PIT->CHANNEL[channel].TCTRL = 0;
	// Initialize PIT0 to count down from argument 
	// Time delay = (period + 1)/24 MHz 
	PIT->CHANNEL[channel].LDVAL = PIT_LDVAL_TSV(period);
	// No chaining
	PIT->CHANNEL[channel].TCTRL &= PIT_TCTRL_CHN_MASK;
	// Do not generate interrupts
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TIE_MASK;
	// Reset interrupt/overflow flag
	PIT->CHANNEL[channel].TFLG = PIT_TFLG_TIF_MASK;
}

void Start_PIT(unsigned channel) {
	// Enable counter
	PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(unsigned channel) {
	// Disable counter
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

unsigned PIT_Expired(unsigned channel) {
	return PIT->CHANNEL[channel].TFLG & PIT_TFLG_TIF_MASK; // Is 1 on underflow (time expired)
}
#endif 

#if (VER==5)
void Task_Read_Switches_Timer(void) {
	PTB->PSOR = MASK(DBG_1);

	if (SWITCH_PRESSED(SW1_POS)) { // flash white
		g_flash_LED = 1;
	} else {
		g_flash_LED = 0;
	}
	if (SWITCH_PRESSED(SW2_POS)) {
		g_w_delay = W_DELAY_FAST;
		g_RGB_delay = RGB_DELAY_FAST;
	}	else {
		g_w_delay = W_DELAY_SLOW;
		g_RGB_delay = RGB_DELAY_SLOW;
	}
	PTB->PCOR = MASK(DBG_1);

}
#endif

#if (VER==5) | (VER==6)
void Task_Flash_FSM_Timer(void) {
	static enum {ST_WHITE, ST_WHITE_WAIT, ST_BLACK, ST_BLACK_WAIT} next_state = ST_WHITE;

	PTB->PSOR = MASK(DBG_2);
	if (g_flash_LED == 1) { 	// Only run task when in flash mode
		switch (next_state) {
			case ST_WHITE:
				Control_RGB_LEDs(1, 1, 1);
				Init_PIT(0, g_w_delay);				
				Start_PIT(0);
				next_state = ST_WHITE_WAIT;
				break;
			case ST_WHITE_WAIT:
				if (PIT_Expired(0)) {
					Stop_PIT(0);
					next_state = ST_BLACK;
				}
				break;
			case ST_BLACK:
				Control_RGB_LEDs(0, 0, 0);
				Init_PIT(0, g_w_delay);				
				Start_PIT(0);
				next_state = ST_BLACK_WAIT;
				break;
			case ST_BLACK_WAIT:
				if (PIT_Expired(0)) {
					Stop_PIT(0);
					next_state = ST_WHITE;
				}
				break;
			default:
				next_state = ST_WHITE;
				break;
		}
	} else {
		next_state = ST_WHITE;
	}
	PTB->PCOR = MASK(DBG_2);
}

void Task_RGB_FSM_Timer(void) {
	static enum {ST_RED, ST_RED_WAIT, ST_GREEN, ST_GREEN_WAIT, ST_BLUE, ST_BLUE_WAIT} next_state = ST_RED;

	PTB->PSOR = MASK(DBG_3);
	if (g_flash_LED == 0) { 	// only run task when NOT in flash mode
		switch (next_state) {
			case ST_RED:
				Control_RGB_LEDs(1, 0, 0);
				Init_PIT(0, g_RGB_delay);				
				Start_PIT(0);
				next_state = ST_RED_WAIT;
				break;	
			case ST_RED_WAIT:
				if (PIT_Expired(0)) {
					Stop_PIT(0);
					next_state = ST_GREEN;
				}
				break;
			case ST_GREEN:
				Control_RGB_LEDs(0, 1, 0);
				Init_PIT(0, g_RGB_delay);				
				Start_PIT(0);
				next_state = ST_GREEN_WAIT;
				break;	
			case ST_GREEN_WAIT:
				if (PIT_Expired(0)) {
					Stop_PIT(0);
					next_state = ST_BLUE;
				}
				break;
			case ST_BLUE:
				Control_RGB_LEDs(0, 0, 1);
				Init_PIT(0, g_RGB_delay);				
				Start_PIT(0);
				next_state = ST_BLUE_WAIT;
				break;	
			case ST_BLUE_WAIT:
				if (PIT_Expired(0)) {
					Stop_PIT(0);
					next_state = ST_RED;
				}
				break;			
			default:
				next_state = ST_RED;
				break;
		}
	} else {
		next_state = ST_RED;
	}
	PTB->PCOR = MASK(DBG_3);
}
#endif

#if (VER==5)
// void Flasher_V4(void) {
void Flasher(void) {
	while (1) {
		Task_Read_Switches_Timer();
		Task_Flash_FSM_Timer();
		Task_RGB_FSM_Timer();
	}
}
#endif

#if (VER==6)
// void Flasher_V5(void) {
void Flasher(void) {	
	while (1) {
		Task_Flash_FSM_Timer();
		Task_RGB_FSM_Timer();
	}
}
#endif


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Initialize_Ports();
	Init_Debug_Signals();
#if (VER==3) | (VER==6)
	Initialize_Interrupts();
	__enable_irq();
#endif
	Flasher();
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
