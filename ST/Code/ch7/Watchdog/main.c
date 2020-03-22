#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include <math.h>
#include "rgb.h"
#include "clock.h"
#include "i2c.h"
#include "inertial_sensor.h"
#include "delay.h"

#define NUM_STARTUP_FLASHES (5)
#define STARTUP_FLASH_DURATION (100000)
#define MAX_ANGLE (30)

WWDG_HandleTypeDef WWDG_Handle;
WWDG_InitTypeDef WWDG_Init;
void Init_WWDG(void) {
	__HAL_RCC_WWDG_CLK_ENABLE();
	// Select the bus clock divided by 4096 * 8
	WWDG_Init.Prescaler = WWDG_PRESCALER_8;
	WWDG_Init.Counter = 0x7F;
	WWDG_Init.Window = 0x7F;
	WWDG_Handle.Init = WWDG_Init;
	WWDG_Handle.Instance = WWDG;
	HAL_WWDG_Init(&WWDG_Handle);
}

void Service_WWDG(void) {
	HAL_WWDG_Refresh(&WWDG_Handle);
}
  
/* Listing 7.4 */
void Flash_Reset_Cause() {
	unsigned int n;
    for (n = 0; n < NUM_STARTUP_FLASHES; n++) {
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
            // Red: WWDG caused reset
            Control_RGB_LEDs(1, 0, 0);
				else
            // Green: WWDG did not cause reset
            Control_RGB_LEDs(0, 1, 0);
        
        Delay(STARTUP_FLASH_DURATION);
        Control_RGB_LEDs(0, 0, 0);
        Delay(2 * STARTUP_FLASH_DURATION);
    }
	  __HAL_RCC_CLEAR_RESET_FLAGS();
}

/* Listing 7.5 */
int main(void) {
  Set_Clocks_To_48MHz();
	Init_GPIO_RGB();
	// Show system is starting up by flashing LEDs
	Flash_Reset_Cause();
	
	// Magenta indicates initialization
	Control_RGB_LEDs(1,0,1);
	Init_I2C();
	// Init inertial sensor
	Init_Inertial_Sensor();
	// LEDs off = done with initialization
	Control_RGB_LEDs(0,0,0); 

	Init_WWDG();
	Service_WWDG();
	
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();		
		if ((fabs(roll) > MAX_ANGLE | fabs(pitch) > MAX_ANGLE))
			// Light yellow LED as warning
			Control_RGB_LEDs(1, 1, 0);
		else {
			// Light green LED - OK!
			Control_RGB_LEDs(0, 1, 0); 
			Service_WWDG();
		}
	}
}
