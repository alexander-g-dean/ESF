#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "field_access.h"
#include "gpio.h"

/* Debug bits show timing of copy operations: 
	PB1 for software copy
	PB2 for DMA copy
*/

void Init_MCO(void) { // Optional code to confirm CPU clock frequency
	// Enable peripheral clock of GPIOA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Alternate function for PA8
	MODIFY_FIELD(GPIOA->MODER, GPIO_MODER_MODER8, ESF_GPIO_MODER_ALT_FUNC);
	// MCO is ALT 0, so selected by default
	MODIFY_FIELD(GPIOA->AFR[1], GPIO_AFRH_AFSEL8, 0);
	
	// Select MCO source 
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
	MODIFY_FIELD(RCC->CFGR, RCC_CFGR_MCOPRE, 2); // Divide by 4
}

// Start Listing DMA_Init_Compare_Arrays
#define ARR_SIZE (256)
uint32_t s[ARR_SIZE], d[ARR_SIZE];

void Init_Arrays(void) {
	uint32_t i;
	for (i = 0; i < ARR_SIZE; i++) {
		s[i] = i;
		d[i] = 0;
	}
}

void Compare_Arrays(void) {
	uint32_t i;
	for (i = 0; i < ARR_SIZE; i++) {
		if (d[i] != s[i]) {
			while (1);								// stop here on error
		}
	}
}
// End Listing DMA_Init_Compare_Arrays

// Start Listing HAL_DMA_Init_DMA_To_Copy
DMA_HandleTypeDef DMA_handle;
void HAL_Init_DMA_To_Copy(void) {
	DMA_InitTypeDef DMA_init;
	__HAL_RCC_DMA1_CLK_ENABLE();

	DMA_init.Mode = DMA_NORMAL;
	DMA_init.Direction = DMA_MEMORY_TO_MEMORY;
	DMA_init.PeriphInc = DMA_PINC_ENABLE;
	DMA_init.MemInc = DMA_MINC_ENABLE;
	DMA_init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	DMA_init.PeriphDataAlignment =
			DMA_PDATAALIGN_WORD;
	DMA_handle.Init = DMA_init;
	DMA_handle.Instance = DMA1_Channel1;
	DMA_handle.ChannelIndex = 1;
	if (HAL_DMA_Init(&DMA_handle) != HAL_OK)
		while (1);
}
// End Listing HAL_DMA_Init_DMA_To_Copy

// Start Listing DMA_Test_DMA_Copy
void Test_DMA_Copy(void) {
	GPIOB->BSRR = GPIO_BSRR_BS_2;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	DMA1_Channel1->CCR = DMA_CCR_MEM2MEM | DMA_CCR_MINC | 
		DMA_CCR_PINC | DMA_CCR_DIR;
	MODIFY_FIELD(DMA1_Channel1->CCR, DMA_CCR_MSIZE, 2);  
	MODIFY_FIELD(DMA1_Channel1->CCR, DMA_CCR_PSIZE, 2);
	MODIFY_FIELD(DMA1_Channel1->CCR, DMA_CCR_PL, 3);
		
	DMA1_Channel1->CNDTR = ARR_SIZE;
	DMA1_Channel1->CMAR = (uint32_t) s;
	DMA1_Channel1->CPAR = (uint32_t) d;
	
	DMA1_Channel1->CCR |= DMA_CCR_EN;	// Enable DMA transfer
	while (!(DMA1->ISR & DMA_ISR_TCIF1))
		;
	DMA1->IFCR = DMA_IFCR_CTCIF1;	// Clear transfer complete flag
	GPIOB->BSRR = GPIO_BSRR_BR_2;
}
// End Listing DMA_Test_DMA_Copy

// Start Listing DMA_Test_SW_Copy
void Test_SW_Copy(void) {
	uint32_t i;

	GPIOB->BSRR = GPIO_BSRR_BS_1;
	for (i = 0; i < ARR_SIZE; i++) {
		d[i] = s[i];
	}
	GPIOB->BSRR = GPIO_BSRR_BR_1;
}
// End Listing DMA_Test_SW_Copy

void Init_GPIO(void) {
	// Enable peripheral clock of GPIOB
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Configure two pins as output
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER1, ESF_GPIO_MODER_OUTPUT);
	MODIFY_FIELD(GPIOB->MODER, GPIO_MODER_MODER2, ESF_GPIO_MODER_OUTPUT);
	// Clear outputs
	GPIOB->BSRR = GPIO_BSRR_BR_1 | GPIO_BSRR_BR_2;
}

void HAL_Init_GPIO(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	// Init PB1 as output
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Init PB2 as output
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,
										GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,
										GPIO_PIN_RESET);
}

// Start Listing DMA_Copy_main
int main(void) {
	Set_Clocks_To_48MHz();
	Init_GPIO();
	//	Init_MCO(); // Confirm CPU clock speed
	while (1) {
		Init_Arrays();
		Test_SW_Copy(); 	// PB1 == 1 when active 
		Compare_Arrays(); // Stop if error
		
		Init_Arrays();
		Test_DMA_Copy(); 	// PB2 == 1 when active
		Compare_Arrays(); // Stop if error
	}
}
// End Listing DMA_Copy_main
