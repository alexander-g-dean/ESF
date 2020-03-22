#include <MKL25Z4.h>

#define LED1_SHIFT (1)	// on port A
#define LED2_SHIFT (2)	// on port A
#define SW1_SHIFT (5)	// on port A

#define MASK(x) (1UL << (x))

// Putting the C Code Together, pp. 40-41
void Basic_Light_Switching_Example(void) {
  // Enable Clock to Port A 
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;          
  
  // Make 3 pins GPIO
  PORTA->PCR[LED1_SHIFT] &= ~PORT_PCR_MUX_MASK;          
  PORTA->PCR[LED1_SHIFT] |= PORT_PCR_MUX(1);          
  PORTA->PCR[LED2_SHIFT] &= ~PORT_PCR_MUX_MASK;          
  PORTA->PCR[LED2_SHIFT] |= PORT_PCR_MUX(1);          
  PORTA->PCR[SW1_SHIFT] &= ~PORT_PCR_MUX_MASK;          
  PORTA->PCR[SW1_SHIFT] |= PORT_PCR_MUX(1);          

  // set LED bits to outputs
  PTA->PDDR |= MASK(LED1_SHIFT) | MASK(LED2_SHIFT); 
  // clear switch bit to input
  PTA->PDDR &= ~MASK(SW1_SHIFT); 
  // turn on LED1, turn off LED2
  PTA->PSOR = MASK(LED1_SHIFT);  
  PTA->PCOR = MASK(LED2_SHIFT);  
    
  while (1) {
	if (PTA->PDIR & MASK(SW1_SHIFT)) {
	  // switch is not pressed, so light only LED 2
	  PTA->PSOR = MASK(LED2_SHIFT);
	  PTA->PCOR = MASK(LED1_SHIFT);
	} else {
	  // switch is pressed, so light only LED 1
	  PTA->PSOR = MASK(LED1_SHIFT);
	  PTA->PCOR = MASK(LED2_SHIFT);
	}
  }
}

// Freedom KL25Z Example: Driving a Three-Color LED, pp. 41-43

void Delay(volatile unsigned int time_del) {
  while (time_del--) {
	;
  }
}

#define RED_LED_SHIFT   (18)	// on port B
#define GREEN_LED_SHIFT (19)	// on port B
#define BLUE_LED_SHIFT  (1)		// on port D


void KL25Z_RGB_Flasher(void) {
	int num;
	// Enable clock to Port B and Port D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_SHIFT] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_SHIFT] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_SHIFT] |= PORT_PCR_MUX(1);  

	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PDDR |= MASK(BLUE_LED_SHIFT); 

	PTB->PCOR |= MASK(RED_LED_SHIFT) | MASK(GREEN_LED_SHIFT);
	PTD->PCOR |= MASK(BLUE_LED_SHIFT);

	while (1) {
	  for (num = 0; num < 8; num++) {
		if (num & 1)
		  PTB->PSOR = MASK(RED_LED_SHIFT);
		else
		  PTB->PCOR = MASK(RED_LED_SHIFT); 
		if (num & 2)
		  PTB->PSOR = MASK(GREEN_LED_SHIFT);
		else
		  PTB->PCOR = MASK(GREEN_LED_SHIFT); 
		if (num & 4)
		  PTD->PSOR = MASK(BLUE_LED_SHIFT);
		else
		  PTD->PCOR = MASK(BLUE_LED_SHIFT); 
		Delay(2000000);       
	  }
	}
}

// Driving a Speaker, pp. 43-44

#define SPKR_SHIFT (0)

void Init_Speaker( void ) {
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; // enable clock for port C
  PORTC->PCR[SPKR_SHIFT] |= PORT_PCR_MUX(1); // select GPIO
  PTC->PDDR |= MASK(SPKR_SHIFT); // set I/O bit -direction to output
  PTC->PDOR |= MASK(SPKR_SHIFT); // set to 1 initially
}

void Beep(void) {
  Init_Speaker();
  while (1) {
    PTC->PTOR = MASK(SPKR_SHIFT);
    Delay(20000);
  }
}


int main(void) {
	Basic_Light_Switching_Example();
	// KL25Z_RGB_Flasher();
}
	



