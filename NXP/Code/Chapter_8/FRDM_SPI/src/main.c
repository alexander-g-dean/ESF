#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"
#include "gpio_aj.h"
#include "sim_aj.h"
#include "spi_aj.h"

extern void	SD_Init_for_SPI(void);


void Test_SPI_Loopback(void) {
	uint8_t out='A', in;
	while (1) {
		in = test_SPIsend(out);
		if (in != out) {
			// Red: error, data doesn't match
			Control_RGB_LEDs(1, 0, 0);
		} else {
			// Green: data matches
			Control_RGB_LEDs(0, 1, 0);
		}
		out++;
		if (out > 'z') 
			out = 'A';
	}
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	volatile uint8_t msg[] = "123456", c='.';
	volatile int d;
	
	// Init_SIM();
	// Init_GPIO();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0, 0, 0);

	//	Init_SPI1();
	SD_Init_for_SPI();
	
	Test_SPI_Loopback();
	
	while (1)
	{
#if 1
		c = test_SPIsend(alpha); // polling
		if(++alpha>'z')
		{
			alpha = 'A';
		}
#if 0
		for (d=0; d<10; d++)
			;
#endif
#else
		SPI1_Send(SPI_LCD , msg, 6);
		for (d=0; d<100000; d++)
			;
#endif
		
	}
}





// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
