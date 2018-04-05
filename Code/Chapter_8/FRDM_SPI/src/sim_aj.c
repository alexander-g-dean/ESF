#include "sim_aj.h"

void Init_SIM(void)
{
		//turn on clock to port C and E
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK;
		// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

}

void adc_sim(void)
{
				//turn on clock to ADC 
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK; 
	
	// select trigger for ADC
	SIM->SOPT7 &= ~SIM_SOPT7_ADC0ALTTRGEN_MASK;
	SIM->SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN_MASK;
	
	SIM->SOPT7 &= ~SIM_SOPT7_ADC0TRGSEL_MASK;
	SIM->SOPT7 |= SIM_SOPT7_ADC0TRGSEL(0x08); // select TPM0 Overflow as trigger source
	
	
}

void tpm_sim(void)
{
		//turn on clock to TPM 
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM1_MASK;
		//set clock source for tpm
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
}

void spi_sim(void)
{
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
}
