#ifndef HBLED_H
#define HBLED_H

#define PWM_PERIOD (42500) // swwting the PWN frequecy to 35 Hz
#define LIM_DUTY_CYCLE (PWM_PERIOD)
#define PWM_HBLED_CHANNEL (4)

#define ADC_SENSE_CHANNEL (8)

#define R_SENSE (2.2)
#define V_REF (3.3)
#define ADC_FULL_SCALE (0x10000)
#define MA_SCALING_FACTOR (1000)

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

#define DAC_POS 30
#define DAC_RESOLUTION 4096
#define VREF_MV 3300

#define MA_TO_DAC_CODE(i) (i*2.2*DAC_RESOLUTION/(VREF_MV))

extern void Control_HBLED(void);
#endif // HBLED_H
