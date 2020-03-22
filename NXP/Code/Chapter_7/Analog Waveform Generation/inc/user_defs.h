#ifndef USER_DEFS_H
#define USER_DEFS_H

#define MASK(x) (1UL << (x))
#define F_TPM_CLOCK (48000000)
#define F_TPM_OVFLW (100000)

// I/O pin assignments

// On Port E
#define DAC_POS (30)

#define STEP_SIZE (16)

// Program characteristics

#define MAX_PERIOD (10000)
#define MIN_PERIOD (100)

#define SQUARE (0)
#define RAMP (1)
#define SINE (2)

#define DAC_RESOLUTION (4096)
#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
