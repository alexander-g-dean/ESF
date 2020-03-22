#ifndef DAC_H
#define DAC_H

#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>

extern DAC_HandleTypeDef DAC_HandleStruct;

void Init_DAC(void);

#endif
