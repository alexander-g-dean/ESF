#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "switch.h"

#define SW1_POS GPIO_PIN_13
#define SW2_POS GPIO_PIN_7

#define W_DELAY_SLOW 10
#define RGB_DELAY_SLOW 20

#define W_DELAY_FAST 5
#define RGB_DELAY_FAST 10

// Initailly don't flash LED, just do RGB sequence
volatile uint8_t g_flash_LED = 0;
// Delay for white flash
volatile uint32_t g_w_delay = W_DELAY_SLOW;
// Delay for RGB sequence
volatile uint32_t g_RGB_delay = RGB_DELAY_SLOW;

void EXTI4_15_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(SW1_POS) != RESET) {
				__HAL_GPIO_EXTI_CLEAR_IT(SW1_POS);
				HAL_GPIO_EXTI_Callback(SW1_POS);
    } 
		if (__HAL_GPIO_EXTI_GET_IT(SW2_POS) != RESET) {
				__HAL_GPIO_EXTI_CLEAR_IT(SW2_POS);
				HAL_GPIO_EXTI_Callback(SW2_POS);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // Read switches
    if (GPIO_Pin == SW1_POS) { 
        if (SWITCH_PRESSED(SW1_POS))
            g_flash_LED = 1; // Flash white
        else
            g_flash_LED = 0; // RGB sequence
    }
    if (GPIO_Pin == SW2_POS) {
        if (SWITCH_PRESSED(SW2_POS)) {
            // Short delays
            g_w_delay = W_DELAY_FAST;
            g_RGB_delay = RGB_DELAY_FAST;
        } else {
            // Long delays
            g_w_delay = W_DELAY_SLOW;
            g_RGB_delay = RGB_DELAY_SLOW;
        }
    }
}

void Initialize_Interrupts(void) {
		GPIO_InitTypeDef GPIO_InitStruct;
		/* Configure GPIO peripheral. Select GPIO and enable pull-up resistors
       and interrupts on all edges for pins connected to switches. */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = SW1_POS | SW2_POS;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING | GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  

    NVIC_SetPriority(EXTI4_15_IRQn, 3);
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn); 
    
    /* Optional: Configure PRIMASK in case interrupts were disabled */
    __enable_irq();
}

int main(void) {
		Initialize_Interrupts();
	  while (1);
}
