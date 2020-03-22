#include <stdint.h>
#include "spi_aj.h"

void delay(int t) {
	volatile int n;
	
	for (n=0; n<t; n++)
		__nop();

}

int SD_Init_for_SPI(void) {
	int n;
	uint8_t in;
	
	// Bit bang SPI pins for power sequence
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Configure PTE pins for GPIO
	// Configure CS pin (PTE4) as GPIO output
	PORTE->PCR[SPI_CS_POS] &= PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI_CS_POS] |= PORT_PCR_MUX(1);
	PTE->PDDR |= 1<<SPI_CS_POS;
	
	// Configure SCK pin (PTE2) as GPIO output
	PORTE->PCR[SPI_SCK_POS] &= PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI_SCK_POS] |= PORT_PCR_MUX(1);
	PTE->PDDR |= 1<<SPI_SCK_POS;

	// Configure MOSI (DI) pin (PTE3) as GPIO output
	PORTE->PCR[SPI_MOSI_POS] &= PORT_PCR_MUX_MASK;
	PORTE->PCR[SPI_MOSI_POS] |= PORT_PCR_MUX(1);
	PTE->PDDR |= 1<<SPI_MOSI_POS;

	// Wait 1 ms
	delay(5000);
	
	// Set CS high	
	PTE->PSOR = 1<<SPI_CS_POS;
	// Set MOSI high	
	PTE->PSOR = 1<<SPI_MOSI_POS;

	
	// Apply 74 or more clock pulses to SCLK at 100-400 kHz
	for (n = 0; n < 150; n++) {
		PTE->PTOR = 1<<SPI_SCK_POS;
		delay(3);
	}

	
	// Now initialize the SPI1 peripheral
	// ...
	Init_SPI1();
	
	//// Software Reset
	// Send CMD0 with CS low to reset card
	// Does CS actually fall to low when transmitting? check.
	in = test_SPIsend(0x40);
	in = test_SPIsend(0x00);
	in = test_SPIsend(0x00);
	in = test_SPIsend(0x00);
	in = test_SPIsend(0x00);
	in = test_SPIsend(0x95);

	in = test_SPIsend(0x00); // read response?
	
	// Card responds R1 with In Idle State bit set (0x01)

//	// Return control of SS to SPI peripheral
//	PORTE->PCR[4] &= PORT_PCR_MUX_MASK;
//	PORTE->PCR[4] |= PORT_PCR_MUX(1);
//	SPI1->C2 |= SPI_C2_MODFEN_MASK;

	while (1)
		;
	
	return 0;
}

int SD_Send_Command(uint8_t command, uint32_t argument) {
	
	
	return 0;
}
