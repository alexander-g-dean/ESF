#ifndef I2C_H
#define I2C_H

#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
extern I2C_HandleTypeDef I2C_Handle;

int Init_I2C(void);
int32_t I2C_ReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
int32_t I2C_WriteReg(void *handle, uint8_t reg, uint8_t *bufp,
                     uint16_t len);
                  
#endif
