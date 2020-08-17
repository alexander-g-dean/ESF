#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#define MASK(x) (1UL << (x))

#define DEBUG0_POS (0) 	// on port B
#define DEBUG1_POS (1) 	// on port B
#define DEBUG2_POS (2) 	// on port B
#define DEBUG3_POS (3) 	// on port B

#define SET_BIT(x) {PTB->PSOR = MASK(x);}
#define CLEAR_BIT(x) {PTB->PCOR = MASK(x);}

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
