#ifndef CONTROL_AJ_H
#define CONTROL_AJ_H

#include <MKL25Z4.H>
#include "pwm_aj.h"

#define D_MAX (uint32_t)((PWM_PERIOD-1)<<16)
#define D_MIN ((uint32_t)(0.08*PWM_PERIOD)<<16)

#define CTL_LOOP_DIVIDER (1)
#define MAX_STEP (5)

#define MCU_VOLT 2.7

#define SET_POINT_2_7 2.7
#define SET_POINT_3_3 3.3
#define SET_POINT_1_7 1.7

#define SET_2_7 (int32_t) (SET_POINT_2_7*4096)
////#define SET_2_7 (int32_t)(65535*SET_POINT_2_7/(MCU_VOLT*2))
#define SET_3_3 (int32_t)(65535*SET_POINT_3_3/(MCU_VOLT*2))
#define SET_1_7 (int32_t)(65535*SET_POINT_1_7/(MCU_VOLT*2))

#define SCALE	(float)((4096.0)*2*MCU_VOLT/65535.0)

 
extern FX_28_4 Kp_PI_28_4;
extern FX_28_4 Ki_PI_28_4;
extern FX_28_4 K1_PI_28_4;
extern FX_28_4 K2_PI_28_4;

extern FX_28_4 K1_PI_MCU ;
extern FX_28_4 K2_PI_MCU;

typedef struct {
#if FXP
//  FX_4_12 d[3];
  uint32_t d[3];   // pure integer 16_0 format
  FX_20_12 e[3];
#else
  float d[3];
  float e[3];
#endif
} CTL_PARMS_T;  

extern CTL_PARMS_T ctl_parms_2_7;
extern CTL_PARMS_T ctl_parms_3_3;
extern CTL_PARMS_T ctl_parms_1_7;

extern FX_20_12 e_2_7;
extern FX_20_12 e_2_7_prev;

extern int32_t set_value_2_7;
extern int32_t set_value_3_3;
extern int32_t set_value_1_7;


extern uint16_t PIControl(uint16_t adc_result, uint8_t ADC_channel);


#endif

