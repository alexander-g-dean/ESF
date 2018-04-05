#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"
#include "sim_aj.h"
#include "spi_aj.h"


uint32_t spi1_tx_count=0;
uint8_t *spi1_tx_address = 0;
uint8_t *spi1_rx_address = 0;


uint8_t SPI1_SendingData=0;
uint8_t SPI1_ReceivingData=0;

uint8_t alpha = 'A';
#define USE_TEST_CODE 1

#if USE_TEST_CODE
void Init_SPI1(void) {
	// enable clock to SPI1
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// disable SPI1 to allow configuration
	SPI1->C1 &= ~SPI_C1_SPE_MASK;

	// set PTE2 as SPI1_SCK -- ALT2
	PORTE->PCR[2] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[2] |= PORT_PCR_MUX(2);
	// set PTE3 as SPI1_MOSI -- ALT5
	PORTE->PCR[3] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[3] |= PORT_PCR_MUX(5);
	// set PTE1 as SPI1_MISO -- ALT5
	PORTE->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[1] |= PORT_PCR_MUX(5);
	// Enable pull-up resistor on input data
	PORTE->PCR[1] |= PORT_PCR_PE_MASK;
	
	// set PTE4 as SPI1_PCS0 -- ALT2
	PORTE->PCR[4] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[4] |= PORT_PCR_MUX(2);

	// Select master mode, enable SS output
	SPI1->C1 = SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK; 
	SPI1->C2 = SPI_C2_MODFEN_MASK;
	// Select active high clock, first edge sample
	SPI1->C1 &= ~SPI_C1_CPHA_MASK;
  SPI1->C1 &= ~SPI_C1_CPOL_MASK;
	
	//BaudRate = BusClock / ((SPPR+1)*2^(SPR+1)) 
	SPI1->BR = SPI_BR_SPPR(7) | SPI_BR_SPR(3);   
	
	// enable SPI1
	SPI1->C1 |= SPI_C1_SPE_MASK;
}

#else
void Init_SPI1(void) {
	spi_sim();
	// disable SPI
	SPI1->C1 &= ~SPI_C1_SPE_MASK;
////	SPI0->C1 &= ~SPI_C1_SPE_MASK;

	// set PTE2 as SPI1_SCK -- ALT2
	PORTE->PCR[2] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[2] |= PORT_PCR_MUX(2);
	
	// set PTE3 as SPI1_MOSI -- ALT5
	PORTE->PCR[3] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[3] |= PORT_PCR_MUX(5);
	
	// set PTE4 as SPI1_PCS0 -- ALT2
	PORTE->PCR[4] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[4] |= PORT_PCR_MUX(2);
	
	// set PTE1 as SPI1_MISO -- ALT5
	PORTE->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[1] |= PORT_PCR_MUX(5);
	
	SPI1->C1 = (SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK); //0x72u; 
	SPI1->C2 = SPI_C2_MODFEN_MASK;// | SPI_C2_SPMIE_MASK;
	SPI1->C1 &= ~(SPI_C1_CPHA_MASK);
  SPI1->C1 &= ~(SPI_C1_CPOL_MASK);
	
	//BaudRate = BusClock / BaudRateDivisor
	//bps div = (SPPR+1)*2^(SPR+1) = 1536,----Tspi--(1536/12.5Mhz)=122.9us
	SPI1->BR = SPI_BR_SPR(1) | SPI_BR_SPPR(3);   
	
	SPI1->C1 |= SPI_C1_SPE_MASK | SPI_C1_SPIE_MASK;// | SPI_C1_SPTIE_MASK; 
//	NVIC_EnableIRQ (SPI1_IRQn);
}
#endif

void SPI1_IRQHandler(void)
{
	uint8_t err_type;
	uint8_t sio_dummy;
	NVIC_ClearPendingIRQ(SPI1_IRQn);
	
	err_type = SPI1->S & SPI_S_MODF_SHIFT;
	if(err_type != 0)
	{
			SPI1_callback_error(err_type);
	}
	else
	{
			if (spi1_tx_count > 0U)
			{
				if(spi1_rx_address != 0)
				{
//					  while(!(SPI1->S & SPI_S_SPRF_MASK)){};  //Wait for SPI recieve full flag to set
					*spi1_rx_address = SPI1->D;
					spi1_rx_address++;
				}
				else
				{
//						while(!(SPI1->S & SPI_S_SPRF_MASK)){};
					sio_dummy = SPI1->D;
				}
				if(spi1_tx_address != 0U)
				{	
//						while(!(SPI1->S & SPI_S_SPTEF_MASK)){};
					SPI1->D = *spi1_tx_address;
					spi1_tx_address++;
				}
				else
				{
//						while(!(SPI1->S & SPI_S_SPTEF_MASK)){};
					SPI1->D = 0xFFu;	
				}
					
				spi1_tx_count--;
			}
			else 
			{
					if (spi1_tx_count == 0U)
					{
//                while(!(SPI1->S & SPI_S_SPRF_MASK)){};
							 *spi1_rx_address = SPI1->D;
					}
					else
					{
//							while(!(SPI1->S & SPI_S_SPRF_MASK)){};
						sio_dummy = SPI1->D;
					}						
			}
			SPI1_callback_sendend();
			SPI1_callback_receiveend();	
	}
}

uint8_t SPI1_Send_Receive(uint8_t * const tx_buf, uint32_t tx_num, uint8_t * const rx_buf)
{
		uint8_t status = MD_OK;	
		spi1_tx_count = tx_num;        /* send data count */
		spi1_tx_address = tx_buf;     /* send buffer pointer */
		spi1_rx_address = rx_buf;     /* receive buffer pointer */
	    
		if(spi1_tx_count < 1U)
		{
			status = MD_ARGERROR;
		}
		else
		{
			NVIC_DisableIRQ (SPI1_IRQn);
			if(spi1_tx_address != 0U)
			{
					while(!(SPI1->S & SPI_S_SPTEF_MASK)){};  //Wait for SPI transmit empty flag to set
							SPI1->D = *spi1_tx_address;    /* started by writing data to SDR[7:0] */
							spi1_tx_address++;
			}
			else
			{
				while(!(SPI1->S & SPI_S_SPTEF_MASK)){};  //Wait for SPI transmit empty flag to set
							SPI1->D = 0xFF;                   /* Send 0xFF  */
			}		

		spi1_tx_count--;
		NVIC_EnableIRQ (SPI1_IRQn);
		}
    return (status);
}



void SPI1_Send(SPI_perif_t aDevice, uint8_t *aData, uint32_t aLength)
{
	volatile int d;
  uint8_t noRXData;
  SPI1_SendingData = 1;
  SPI1_ReceivingData = 0;
	SPI_CS_Start(aDevice);
	for(d=100;d>0;d--){
	};
  SPI1_Send_Receive(aData, aLength, &noRXData);
	while(SPI1_SendingData)
		;
	SPI_CS_End(aDevice);
}

void SPI1_SendReceive(SPI_perif_t aDevice, uint8_t *aTXData, uint32_t aLength,uint8_t *aRXData)
{
	volatile int d;
  SPI1_SendingData = 1;
  SPI1_ReceivingData = 1;
  SPI_CS_Start(aDevice);
				for(d=100;d>0;d--){};
  SPI1_Send_Receive(aTXData, aLength, aRXData);
	while(SPI1_SendingData || SPI1_ReceivingData);
	SPI_CS_End(aDevice);
}

void SPI_CS_Start(SPI_perif_t aDevice)
{
	switch(aDevice)
	{
		case SPI_LCD:
			PTE->PCOR |= MASK(5);
			SPI1->C1 |= (SPI_C1_CPHA_MASK);
			SPI1->C1 |= (SPI_C1_CPOL_MASK);
			break;
		case SPI_SD:
			break;
		case SPI_WIFI:
			PTB->PCOR  |= MASK(9);
			SPI1->C1 &= ~(SPI_C1_CPHA_MASK);
			SPI1->C1 &= ~(SPI_C1_CPOL_MASK);
			break;
	}	
}

void SPI_CS_End(SPI_perif_t aDevice)
{
	switch(aDevice)
	{
		case SPI_LCD:
			PTE->PSOR |= MASK(5);
		  SPI1->C1 &= ~(SPI_C1_CPHA_MASK);
			SPI1->C1 &= ~(SPI_C1_CPOL_MASK);
			break;
		case SPI_SD:
			break;
		case SPI_WIFI:
			PTB->PSOR  = MASK(9);
			SPI1->C1 &= ~(SPI_C1_CPHA_MASK);
			SPI1->C1 &= ~(SPI_C1_CPOL_MASK);
			break;
	}	
}
uint8_t test_SPIsend(uint8_t d_out)
{
	while(!(SPI1->S & SPI_S_SPTEF_MASK))
		;  //Wait for transmit buffer empty 
	SPI1->D = d_out;

	while (!(SPI1->S & SPI_S_SPRF_MASK))
		; // wait for receive buffer full
	return SPI1->D;
}

void SPI1_callback_receiveend()
{
		SPI1_ReceivingData=0;
}

void SPI1_callback_sendend()
{
		SPI1_SendingData = 0;
}

void SPI1_callback_error(uint8_t err_type)
{
	
}


