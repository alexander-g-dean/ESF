#include <math.h>
#include "lsm6dso_reg.h"
#include "inertial_sensor.h"
#include "i2c.h"


// Start Listing_Init_Sensor
#define LSM6DSL_DEV_ADX 					(0x6b)
#define LSM6DSL_REG_ADX_WHO_AM_I 	(0x0f)
#define LSM6DSL_WHO_AM_I_CODE			(0x6a)

#define LSM6DSL_REG_ADX_CTRL1_XL 	(0x10)
#define LSM6DSL_REG_ADX_CTRL3_C 	(0x12)
#define LSM6DSL_REG_ADX_OUTX_L_XL	(0x28)

void Init_Inertial_Sensor(void) {
	uint8_t data;

	I2C_ReadReg(LSM6DSL_DEV_ADX, LSM6DSL_REG_ADX_WHO_AM_I, &data, 1); 
	if (data != LSM6DSL_WHO_AM_I_CODE)
		while (1); // Halt: sensor not found

	// CTRL3 
	// IF_INC = 1: Enable auto-increment register address
	// BDU = 1: Enable block data update
	data = 0x44;
	I2C_WriteReg(LSM6DSL_DEV_ADX, LSM6DSL_REG_ADX_CTRL3_C, &data, 1); 

	// CTRL1_XL 
	// FS_XL = 00: 2G full scale 
	// ODR_XL = 0100: 104Hz data rate
	data = 0x40;
	I2C_WriteReg(LSM6DSL_DEV_ADX, LSM6DSL_REG_ADX_CTRL1_XL, &data, 1); 
}
// End Listing_Init_Sensor

// Start Listing Sensor_Read_Math
#define SENSITIVITY_AT_2G (0.061f) // mg per LSB
#define M_PI 							(3.14159f)

float g_roll, g_pitch;
int32_t g_x, g_y, g_z;

void read_full_xyz(void) {
	int16_t raw_accel[3];
	
	I2C_ReadReg(LSM6DSL_DEV_ADX, LSM6DSL_REG_ADX_OUTX_L_XL, 
		(uint8_t *) raw_accel, 6);
	/* Convert the data. */
  g_x = (int32_t)((float)raw_accel[0] * SENSITIVITY_AT_2G);
  g_y = (int32_t)((float)raw_accel[1] * SENSITIVITY_AT_2G);
  g_z = (int32_t)((float)raw_accel[2] * SENSITIVITY_AT_2G);
}

void convert_xyz_to_roll_pitch(void) {
	g_roll = atan2f(g_y, g_z)*180/M_PI;
	g_pitch = atan2f(-g_x, sqrtf(g_y*g_y + g_z*g_z))*180/M_PI;
}
// End Listing Sensor_Read_Math


lsm6dso_ctx_t bus;

void HAL_Init_Inertial_Sensor(void) {
	bus.read_reg = HAL_I2C_ReadReg;
	bus.write_reg = HAL_I2C_WriteReg;
	bus.handle = &I2C_Handle;
	
  // Enable auto-increment register address
	lsm6dso_auto_increment_set(&bus, PROPERTY_ENABLE);
	// Enable block data update
  lsm6dso_block_data_update_set(&bus, PROPERTY_ENABLE);
	// 2G full scale and 104Hz data rate
	lsm6dso_xl_full_scale_set(&bus, LSM6DSO_2g);
	lsm6dso_xl_data_rate_set(&bus, LSM6DSO_XL_ODR_104Hz);	
}

void HAL_read_full_xyz(void) {
  axis3bit16_t data_raw;
	
  /* Read raw data values. */
  lsm6dso_acceleration_raw_get(&bus, data_raw.u8bit);
	
  /* Format the data. */
  g_x = (int32_t)((float)data_raw.i16bit[0] * SENSITIVITY_AT_2G);
  g_y = (int32_t)((float)data_raw.i16bit[1] * SENSITIVITY_AT_2G);
  g_z = (int32_t)((float)data_raw.i16bit[2] * SENSITIVITY_AT_2G);
}

