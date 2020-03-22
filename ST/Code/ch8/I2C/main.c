#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "inertial_sensor.h"
#include "i2c.h"

int main(void) {
	Set_Clocks_To_48MHz();
	Init_I2C();
	Init_Inertial_Sensor();
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		// Use the pitch and roll here
	}
}
