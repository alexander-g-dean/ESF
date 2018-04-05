#ifndef USER_DEFS_H
#define USER_DEFS_H

#define MASK(x) (1UL << (x))

// I/O pin assignments

// On Port E
#define SW_UP_POS (21)
#define SW_UP_MSK (1UL << SW_UP_POS)

// On Port E

// Program characteristics

#define MAX_PERIOD (10000)
#define MIN_PERIOD (100)

#define SQUARE (0)
#define RAMP (1)

#define DAC_RESOLUTION (4096)
#define NUM_STEPS (64)

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
