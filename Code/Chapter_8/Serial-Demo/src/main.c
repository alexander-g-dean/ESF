/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "UART.h"

/*----------------------------------------------------------------------------
  MAIN function
	
	Change definition of USE_UART_INTERRUPTS in UART.h to select polled or 
	interrupt-driven communication.
	 *----------------------------------------------------------------------------*/
int main (void) {
	uint8_t buffer[80], c, * bp;

	Init_UART0(115200);
	Init_RGB_LEDs();

 	printf("\n\rGood morning!\n\r");
	
#if USE_UART_INTERRUPTS==0 // Polling version of code
	Send_String_Poll("\n\rHello, World!\n\r");

	// Code listing 8.9, p. 233
	while (1) {
		c = UART0_Receive_Poll();
		Control_RGB_LEDs(0, 0, 1);
		UART0_Transmit_Poll(c+1);
		Control_RGB_LEDs(0, 0, 0);
	}
#elif USE_UART_INTERRUPTS==1 // Interrupt version of code
	Send_String("\n\rHello, World!\n\r");

	// Code listing 8.10, p. 234
	while (1) {
		// Blocking receive
		while (Q_Size(&RxQ) == 0)
			; // wait for character to arrive
		c = Q_Dequeue(&RxQ);
		
		// Blocking transmit
		sprintf((char *) buffer, "You pressed %c\n\r", c);
		// enqueue string
		bp = buffer;
		while (*bp != '\0') { 
			// copy characters up to null terminator
			while (Q_Full(&TxQ))
				; // wait for space to open up
			Q_Enqueue(&TxQ, *bp);
			bp++;
		}
		// start transmitter if it isn't already running
		if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
			UART0->C2 |= UART0_C2_TIE(1);
		}
	}
#endif	

}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
