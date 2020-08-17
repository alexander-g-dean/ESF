#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <MKL25Z4.H>


#define UART_OVERSAMPLE_RATE 	(16)
#define SYS_CLOCK							(48e6)

void Init_UART0(uint32_t baud_rate);
void UART0_Transmit_Poll(uint8_t data);
uint8_t UART0_Receive_Poll(void);

void Send_String_Poll(uint8_t * str);
#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
