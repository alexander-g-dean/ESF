// Start Listing 2.Beep
#include <stm32f091xc.h>
#include "delay.h"

#define SPKR_HIGH (GPIO_BSRR_BS_6)
#define SPKR_LOW (GPIO_BSRR_BR_6)

void Init_Speaker(void) {
	// Enable peripheral clock of GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Configure Port B pin 6 as output (mode 1)
	GPIOB->MODER &= GPIO_MODER_MODER6_Msk;
	GPIOB->MODER |= _VAL2FLD(GPIO_MODER_MODER6, 1);
	// Set to 1 initially
	GPIOB->BSRR |= SPKR_HIGH;
}

void Beep(void) {
	Init_Speaker();
	while (1) {
		// Delay is about 2 ms
		GPIOB->BSRR |= SPKR_HIGH;
		Delay(1800);
		GPIOB->BSRR |= SPKR_LOW;
		Delay(1800);
	}
}
// End Listing 2.Beep

int main(void) {
	Beep();
}

/**
  \brief   Mask and shift a bit field value for use in a register bit range.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit filed value.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of register. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)