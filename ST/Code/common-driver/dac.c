#include <stm32f091xc.h>
#include "dac.h"
#include "field_access.h"
#include "gpio.h"

// Start Listing 6.1
void Init_DAC(void) {
	// Enable clocks for DAC and PA4
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
  // Init PA4 as analog by setting both MODER bits
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER4, ESF_GPIO_MODER_ANALOG);

  // Init DAC1, connecting it to PA4
	DAC1->CR = DAC_CR_EN1;
}
// End Listing 6.1
