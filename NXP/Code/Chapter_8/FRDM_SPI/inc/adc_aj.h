#ifndef ADC_AJ_H
#define ADC_AJ_H

/**************Global Variables*******************/


extern int channel;
extern int flag;
extern signed short forwardD;
extern int UpperBound,LowerBound;
extern float vol;

/*************************************************/


extern void Init_ADC(void);

extern void ADC0_IRQHandler(void);

#endif
