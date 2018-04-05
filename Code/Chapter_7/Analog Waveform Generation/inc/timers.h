#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

void Init_PIT(unsigned period);
void Start_PIT(void);
void Stop_PIT(void);

void Init_TPM(void);


extern volatile unsigned PIT_interrupt_counter;

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
