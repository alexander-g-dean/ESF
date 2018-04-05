#include <stdint.h>
#include <MKL25Z4.h>
#include "timers.h"
#include "LEDs.h"

#define ARR_SIZE (256)
uint32_t s[ARR_SIZE], d[ARR_SIZE];

uint16_t * Reload_DMA_Source=0;
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;

void Init_DMA_To_Copy(void) {
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	DMA0->DMA[0].DCR = DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(0) |
		DMA_DCR_DINC_MASK |	DMA_DCR_DSIZE(0);
}

void Copy_Longwords(uint32_t * source, uint32_t * dest, uint32_t count) {
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) source);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) dest);
	// initialize byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(count*4);
	// clear done flag and status flags
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 

	// start transfer
	Control_RGB_LEDs(0,0,1);
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
	// mark start of transfer
	Control_RGB_LEDs(0,0,0);
	Control_RGB_LEDs(0,0,1);
	// wait until it is done
	while (!(DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK))
		;
	// mark end of transfer
	Control_RGB_LEDs(0,0,0);
}

void Test_DMA_Copy(void) {
	uint16_t i;
	
	Init_DMA_To_Copy();
	
	for (i=0; i<ARR_SIZE; i++) {
		s[i] = i;
		d[i] = 0;
	}
	Copy_Longwords(s, d, ARR_SIZE);	
}


void Init_DMA_For_Playback(uint16_t * source, uint32_t count) {
	// Save reload information
	Reload_DMA_Source = source;
	Reload_DMA_Byte_Count = count*2;
	
	// Gate clocks to DMA and DMAMUX
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	// Disable DMA channel to allow configuration
	DMAMUX0->CHCFG[0] = 0;
	
	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK | 
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;
	
	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_ClearPendingIRQ(DMA0_IRQn); 
	NVIC_EnableIRQ(DMA0_IRQn);	

	// Enable DMA MUX channel with TPM0 overflow as trigger
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(54);   
}

void Start_DMA_Playback() {
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	// clear done flag 
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK; 
	// set enable flag
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;

}

void DMA0_IRQHandler(void) {
	// Turn off blue LED in DMA IRQ handler
	Control_RGB_LEDs(0,0,0);
	// Clear done flag 
	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK; 
	// Start the next DMA playback cycle
	Start_DMA_Playback();
	// Turn on blue LED
	Control_RGB_LEDs(0,0,1);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
