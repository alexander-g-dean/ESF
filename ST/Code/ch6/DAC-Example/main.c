#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "dac.h"

#define DAC_RESOLUTION 4095

void Triangle_Output(void) {
    int i = 0;
    int change = 1;
    
    while (1) {
        HAL_DAC_SetValue(&DAC_HandleStruct, DAC_CHANNEL_1, DAC_ALIGN_12B_R, i);
    
        i += change;
        if (i <= 0)
            change = 1;
        else if (i >= DAC_RESOLUTION - 1)
            change = -1;
    }
}

int main(void) {
    Init_DAC();
	  Triangle_Output();
}
