#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "lsm6dso_reg.h"
#include "i2c.h"

I2C_HandleTypeDef I2C_Handle;

int Init_I2C(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_Init;

  // Clock gating for I2C1 and GPIO B
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  // GPIO B pin 8 and 9 in alternate function 1
  GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF1_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C_Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	I2C_Init.Timing = (uint32_t) 0x00B01A4B;
	I2C_Handle.Mode = HAL_I2C_MODE_MASTER;
	I2C_Handle.Instance = I2C1;
	I2C_Handle.Init = I2C_Init;
	
	return HAL_I2C_Init(&I2C_Handle);
}

int32_t I2C_ReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	if (HAL_I2C_Mem_Read(handle, LSM6DSO_I2C_ADD_H, 
		reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 0xFFF) != HAL_OK)
		while (1);
	return 0;
}
int32_t I2C_WriteReg(void *handle, uint8_t reg, uint8_t *bufp,
                     uint16_t len) {
		if (HAL_I2C_Mem_Write(handle, LSM6DSO_I2C_ADD_H, 
		reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 0xFFF) != HAL_OK)
		while (1);
	return 0;
}
