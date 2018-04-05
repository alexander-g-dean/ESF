#ifndef PWM_AJ_H
#define PWM_AJ_H

#include <MKL25Z4.H>

#define PWM_PERIOD (959)

extern void Init_PWM(void);

extern void TPM0_IRQHandler(void);



#endif
