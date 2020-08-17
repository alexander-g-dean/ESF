#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

void PWM_Init(TPM_Type * TPM, uint8_t channel_num, uint16_t period, uint16_t duty);
void PWM_Set_Value(TPM_Type * TPM, uint8_t channel_num, uint16_t value);

void PIT_Init(unsigned period);
void PIT_Start(void);



#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
