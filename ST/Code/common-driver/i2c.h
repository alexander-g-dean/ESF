#ifndef I2C_H
#define I2C_H

#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
extern I2C_HandleTypeDef I2C_Handle;

int HAL_Init_I2C(void);
int32_t HAL_I2C_ReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t HAL_I2C_WriteReg(void *handle, uint8_t reg, uint8_t *bufp,
                     uint16_t len);

void I2C_Init(void);
void I2C_WriteReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp,
uint16_t data_len);

void I2C_ReadReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp,
uint16_t data_len);

#endif
