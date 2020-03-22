#include <stm32f091xc.h>
#include <stm32f0xx_hal.h>
#include "clock.h"
#include "rgb.h"

#define MAX_DAC_CODE (4095)
#define NUM_STEPS (512)

#define TIM_PERIOD (16) 
#define TIM_PRESCALER (32)

DMA_HandleTypeDef DMA_handle;

uint16_t TriangleTable[NUM_STEPS];
void Init_TriangleTable(void) {
    unsigned n, sample = 0;
    
    for (n = 0; n < NUM_STEPS / 2; n++) {
			  sample = (n * (MAX_DAC_CODE + 1) / (NUM_STEPS / 2));
        // Fill in from front
        TriangleTable[n] = sample;
        // Fill in from back
        TriangleTable[NUM_STEPS - 1 - n] = sample;
    }
}

DAC_HandleTypeDef DAC_HandleStruct;
void Init_DAC(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    DAC_ChannelConfTypeDef DAC_Conf;
		
	  __HAL_RCC_DAC1_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	
    // Init PA4 as analog output
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Init DAC1
	  DAC_HandleStruct.Instance = DAC1;
	  // The DAC is triggered by TIM6
    DAC_Conf.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	  // The DAC needs a reference to the DMA handle
		DAC_HandleStruct.DMA_Handle1 = &DMA_handle;
    DAC_Conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_Init(&DAC_HandleStruct);
		HAL_DAC_ConfigChannel(&DAC_HandleStruct, &DAC_Conf, DAC_CHANNEL_1);
	  HAL_DAC_Start(&DAC_HandleStruct, DAC_CHANNEL_1);
}

	
void DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler(void) {
	if (DMA2->ISR & DMA_FLAG_TC3) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

	}
	DMA2->IFCR |= DMA_FLAG_TC3 | DMA_FLAG_HT3 | DMA_FLAG_TE3;
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

void Init_DMA_For_Playback(void) {
	DMA_InitTypeDef DMA_init;
	
	// Enable DMA2 clock
	__HAL_RCC_DMA2_CLK_ENABLE();
	// Memory to peripheral mode, 16-bit data, circular mode
	DMA_init.Direction = DMA_MEMORY_TO_PERIPH;
	DMA_init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMA_init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMA_init.Mode = DMA_CIRCULAR;

	// Auto-increment enable for memory
	DMA_init.MemInc = DMA_MINC_ENABLE;
	DMA_init.PeriphInc = DMA_PINC_DISABLE;
	DMA_init.Priority = DMA_PRIORITY_VERY_HIGH;
	DMA_handle.ChannelIndex = 3;
	DMA_handle.Init = DMA_init;
	DMA_handle.Instance = DMA2_Channel3;
	
	// DMA request is mapped to DAC channel 1
	__HAL_DMA2_REMAP(HAL_DMA2_CH3_DAC_CH1);
	if (HAL_DMA_Init(&DMA_handle) != HAL_OK) 
		while (1);

	NVIC_SetPriority(DMA1_Ch4_7_DMA2_Ch3_5_IRQn, 3);
  NVIC_ClearPendingIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
  NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
}

void Start_DMA(uint32_t *source, uint32_t length) {
  HAL_DAC_Start_DMA(&DAC_HandleStruct, DAC_CHANNEL_1, source, length, DAC_ALIGN_12B_R);
}

TIM_HandleTypeDef TIM_HandleStruct;
void Init_TIM(void) {
	TIM_Base_InitTypeDef TIM_InitStruct; 
  TIM_MasterConfigTypeDef TIM_Master;
	__HAL_RCC_TIM6_CLK_ENABLE();
	
	TIM_InitStruct.Period = TIM_PERIOD - 1; 
	TIM_InitStruct.Prescaler = TIM_PRESCALER - 1; 
	TIM_InitStruct.CounterMode = TIM_COUNTERMODE_UP;	
	// Enable trigger upon an update event
	TIM_Master.MasterOutputTrigger = TIM_TRGO_UPDATE;
	
	TIM_HandleStruct.Instance = TIM6;
	TIM_HandleStruct.Init = TIM_InitStruct;

	HAL_TIMEx_MasterConfigSynchronization(&TIM_HandleStruct, &TIM_Master);
	HAL_TIM_Base_Init(&TIM_HandleStruct);
	HAL_TIM_Base_Start(&TIM_HandleStruct);
}

void Init_GPIO(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Init PB1 as output for debug
	__HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Play_Tone_With_DMA(void) {
	Init_GPIO_RGB();
	Control_RGB_LEDs(0, 0, 0);
	Init_TriangleTable();
	Init_DMA_For_Playback();
	Init_TIM();
	Init_DAC();
	Start_DMA((uint32_t *)TriangleTable, NUM_STEPS);
	while (1);
}

int main(void) {
	Set_Clocks_To_48MHz();
	Init_GPIO();
  Play_Tone_With_DMA();
}
