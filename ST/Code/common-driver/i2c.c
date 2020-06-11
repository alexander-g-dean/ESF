#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "lsm6dso_reg.h"
#include "i2c.h"
#include "field_access.h"

I2C_HandleTypeDef I2C_Handle;

int HAL_Init_I2C(void) {
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
// Start Listing I2C_Init
void I2C_Init(void) {
  // Clock gating for I2C1 and GPIO B
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	
  // GPIO B pin 8 and 9 in alternate function
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER8, 2);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER9, 2);
	// Select I2C1 (AF = 1) as alternate function
	MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL8, 1);
	MODIFY_FIELD(GPIOB->AFR[1], GPIO_AFRH_AFSEL9, 1);	
	
	// I2C1 Configuration
	I2C1->CR1 = 0; // Default configuration, peripheral disabled
	I2C1->CR2 = 0; // Default configuration, 7 bit addressing
	I2C1->TIMINGR = 0x00B01A4B; // about 71 kbaud
	
	MODIFY_FIELD(I2C1->CR1, I2C_CR1_PE, 1); // Enable peripheral
}
// End Listing I2C_Init

// Start Listing I2C_WriteReg
void I2C_WriteReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp,
	uint16_t data_len) {
	uint32_t tmp;
	// -- Send START, Device Address, Write Command --
	tmp = 0;
	MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx << 1);
	MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 0); // Write
	MODIFY_FIELD(tmp, I2C_CR2_NBYTES, data_len+1); //data bytes + reg. adx.
	// Set START to start transfer
	MODIFY_FIELD(tmp, I2C_CR2_START, 1);
	I2C1->CR2 = tmp;
	// Wait until START is cleared by hardware
	while (I2C1->CR2 & I2C_CR2_START)
		;
	// -- Send Register Address --
	I2C1->TXDR = reg_adx;
	// Wait until transmitter empty
	while (!(I2C1->ISR & I2C_ISR_TXE))
		;
	// -- Send Data --
	while (data_len--) {
		I2C1->TXDR = *bufp;
		bufp++;
		while (!(I2C1->ISR & I2C_ISR_TXE))
			;
	}
	// -- Send Stop --
	MODIFY_FIELD(I2C1->CR2, I2C_CR2_STOP, 1);
}
// End Listing I2C_WriteReg

void test_I2CWrite(void) {
	uint8_t data[] = {0, 0, 0, 0, 0}; 
	
	I2C_WriteReg(0x6B, 0x06, data, 5);  
}


// Start Listing I2C_ReadReg
void I2C_ReadReg(uint8_t dev_adx, uint8_t reg_adx, uint8_t *bufp, 
	uint16_t data_len) {
	uint32_t tmp;
	// -- Send START, Device Address, Write Command --
	tmp = 0;
	MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx << 1);
	MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 0); // First write addresses
	MODIFY_FIELD(tmp, I2C_CR2_NBYTES, 1); // 1 byte: register address
	MODIFY_FIELD(tmp, I2C_CR2_START, 1); 	// Start transfer
	I2C1->CR2 = tmp;
	while (I2C1->CR2 & I2C_CR2_START) // Wait until START ends
	 ;
	// -- Write Register Address --
	I2C1->TXDR = reg_adx;
	while (!(I2C1->ISR & I2C_ISR_TXE)) // Wait until sent
		;
	// -- Send Repeated START, Device Address, Read Command--
	tmp = I2C1->CR2;
	MODIFY_FIELD(tmp, I2C_CR2_SADD, dev_adx << 1);
	MODIFY_FIELD(tmp, I2C_CR2_RD_WRN, 1);	// Then read data
	MODIFY_FIELD(tmp, I2C_CR2_NBYTES, data_len); // data byte count
	MODIFY_FIELD(tmp, I2C_CR2_START, 1); // Start transfer
	I2C1->CR2 = tmp;
	// -- Read Data --
	while (data_len--) { 
		while (!(I2C1->ISR & I2C_ISR_RXNE)) // Await data rx
			;
		*bufp++ = I2C1->RXDR; // Read RXDR, advance pointer
	}
	// -- Write Stop --
	MODIFY_FIELD(I2C1->CR2, I2C_CR2_STOP, 1);
}
// End Listing I2C_ReadReg

int32_t HAL_I2C_WriteReg(void *handle, uint8_t reg, uint8_t *bufp,
                     uint16_t len) {
		if (HAL_I2C_Mem_Write(handle, LSM6DSO_I2C_ADD_H, 
		reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 0xFFF) != HAL_OK)
		while (1);
	return 0;
}

int32_t HAL_I2C_ReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
	if (HAL_I2C_Mem_Read(handle, LSM6DSO_I2C_ADD_H, 
		reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 0xFFF) != HAL_OK)
		while (1);
	return 0;
}
