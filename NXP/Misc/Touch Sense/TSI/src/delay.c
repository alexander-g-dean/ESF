#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"

void SysTick_Init(void){
  SysTick->CTRL = 0;               // disable SysTick during setup
  SysTick->CTRL = 0x00000005;      // enable SysTick with core clock and not using interrupt
}

void SysTick_Wait(uint16_t delay){
  SysTick->LOAD = delay-1;  // number of counts to wait
  SysTick->VAL = 0;       // any value written to CURRENT clears it
  while((SysTick->CTRL&0x00010000)==0){ // wait for count flag
  }
}

// this function wait is 1 ms if the delay input is 1.
// dealy input can change from 1 to 600000.
void SysTick_Wait1ms(uint16_t delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(48000);  // wait 1ms 
		// 48000 is the count calculated based on the bus clock and the wait time required that is 1msec
  }
}

