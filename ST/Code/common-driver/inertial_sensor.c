#include <math.h>
#include "lsm6dso_reg.h"
#include "inertial_sensor.h"
#include "i2c.h"

#define SENSITIVITY_AT_2G (0.061f)
#define M_PI (3.14159)

int32_t x, y, z;
double roll, pitch;
lsm6dso_ctx_t bus;

void convert_xyz_to_roll_pitch(void) {
	roll = atan2(y, z)*180/M_PI;
	pitch = atan2(-x, sqrt(y*y + z*z))*180/M_PI;
}

void read_full_xyz(void) {
  axis3bit16_t data_raw;
	
  /* Read raw data values. */
  lsm6dso_acceleration_raw_get(&bus, data_raw.u8bit);
	
  /* Format the data. */
  x = (int32_t)((float)data_raw.i16bit[0] * SENSITIVITY_AT_2G);
  y = (int32_t)((float)data_raw.i16bit[1] * SENSITIVITY_AT_2G);
  z = (int32_t)((float)data_raw.i16bit[2] * SENSITIVITY_AT_2G);
}

void Init_Inertial_Sensor(void) {
	bus.read_reg = I2C_ReadReg;
	bus.write_reg = I2C_WriteReg;
	bus.handle = &I2C_Handle;
	
  // Enable auto-increment register address
	lsm6dso_auto_increment_set(&bus, PROPERTY_ENABLE);
	// Enable block data update
  lsm6dso_block_data_update_set(&bus, PROPERTY_ENABLE);
	// 2G full scale and 104Hz data rate
	lsm6dso_xl_full_scale_set(&bus, LSM6DSO_2g);
	lsm6dso_xl_data_rate_set(&bus, LSM6DSO_XL_ODR_104Hz);	
}
