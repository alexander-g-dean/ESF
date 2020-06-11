#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "inertial_sensor.h"
#include "i2c.h"
#include "rgb.h"
#include "math.h"

// Start Listing I2C_main
#define ROLL_MAX_DEG (2.0)
#define PITCH_MAX_DEG (2.0)

int main(void) {
	int red, green, blue;
	
	Set_Clocks_To_48MHz();
	Init_GPIO_RGB();
	Control_RGB_LEDs(0,0,0);
	I2C_Init();
	Init_Inertial_Sensor();
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		red = (g_roll > ROLL_MAX_DEG) || (g_roll < -ROLL_MAX_DEG)? 1 : 0;
		blue = (g_pitch > PITCH_MAX_DEG) || (g_pitch < -PITCH_MAX_DEG)? 1 : 0;
		green = (red | blue)? 0 : 1;
		Control_RGB_LEDs(red, green, blue);
	}
}
// End Listing I2C_main
