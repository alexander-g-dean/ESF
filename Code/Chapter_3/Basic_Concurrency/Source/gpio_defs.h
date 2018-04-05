#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#define SW1_POS (7)		// on port D
#define SW2_POS (6)		// on port D

#define MASK(x) (1UL << (x))

// Debug Signals on port B
#define DBG_1 1
#define DBG_2 2
#define DBG_3 3
#define DBG_4 8
#define DBG_5 9
#define DBG_6 10
#define DBG_7 11


#define SWITCH_PRESSED(x) (!(PTD->PDIR & (MASK(x))))

// Freedom KL25Z LEDs
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)		// on port B
#define BLUE_LED_POS (1)		// on port D

#define ON_TIME (4000)
#define FLASH_TIME (600)

/*
	Code phases:
			1: Base with busy waits
			2: Tasks
			3: Interrupt to read switches
			4: FSM LED tasks
			5: Add HW Timer to FSM LED and switch tasks
			6: Interrupt to read switches
*/

#define VER (6) 


#if (VER==1) | (VER==2) | (VER==3) | (VER==4)
#define W_DELAY_SLOW 400
#define W_DELAY_FAST 200
#define RGB_DELAY_SLOW 6000
#define RGB_DELAY_FAST 1000
#else
#define W_DELAY_SLOW 2000000
#define W_DELAY_FAST 1000000
#define RGB_DELAY_SLOW 36000000
#define RGB_DELAY_FAST 8000000
#endif

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
