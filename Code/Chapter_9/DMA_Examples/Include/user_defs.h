#ifndef USER_DEFS_H
#define USER_DEFS_H

#define MASK(x) (1UL << (x))

// I/O pin assignments

// On Port E
#define DAC_POS (30)

// Program characteristics

// #define M_BUSY_WAIT 
// #define M_INTERRUPTS
#define M_DMA 1

#define SQUARE (0)
#define RAMP (1)
#define SINE (2)

#define MAX_DAC_CODE (4095)
#define NUM_STEPS (512)

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
