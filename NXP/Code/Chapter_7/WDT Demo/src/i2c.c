#include <MKL25Z4.H>
#include "i2c.h"
int lock_detect=0;
int i2c_lock=0;

#define ENABLE_I2C_RESET 1

//init i2c0
void i2c_init(void)
{
	//clock i2c peripheral and port E
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK);
	
	//set pins to I2C function
	PORTE->PCR[24] |= PORT_PCR_MUX(5);
	PORTE->PCR[25] |= PORT_PCR_MUX(5);
		
	// set to 400k baud
	// baud = bus freq/(scl_div+mul)
 	// 24MHz/400kHz = 60; icr=0x11 sets scl_div to 56
 	I2C0->F = (I2C_F_ICR(0x11) | I2C_F_MULT(0));
		
	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK);
	
	//select high drive mode
	I2C0->C2 |= (I2C_C2_HDRS_MASK);
}


void i2c_busy(void){
	// Start Signal
	lock_detect=0;
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C_TRAN;
	I2C_M_START;
	I2C0->C1 |=  I2C_C1_IICEN_MASK;
	
	// Write to clear line
	I2C0->C1 |= I2C_C1_MST_MASK; /* set MASTER mode */  
	I2C0->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */  
	I2C0->D = 0xFF;
	while ((I2C0->S & I2C_S_IICIF_MASK) == 0U) {
	} /* wait interrupt */  
	I2C0->S |= I2C_S_IICIF_MASK; /* clear interrupt bit */  

							/* Clear arbitration error flag*/  
	I2C0->S |= I2C_S_ARBL_MASK;
		
		
							/* Send start */  
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */  
	I2C0->C1 |= I2C_C1_MST_MASK; /* START signal generated */  
		
	I2C0->C1 |= I2C_C1_IICEN_MASK;
							/*Wait until start is send*/  

							/* Send stop */  
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C0->C1 |= I2C_C1_MST_MASK;
	I2C0->C1 &= ~I2C_C1_MST_MASK; /* set SLAVE mode */  
	I2C0->C1 &= ~I2C_C1_TX_MASK; /* Set Rx */  
	I2C0->C1 |= I2C_C1_IICEN_MASK;
		
	
								/* wait */  
							/* Clear arbitration error & interrupt flag*/  
	I2C0->S |= I2C_S_IICIF_MASK;
	I2C0->S |= I2C_S_ARBL_MASK;
	lock_detect=0;
	i2c_lock=1;
}

#ifdef ENABLE_I2C_RESET
#pragma no_inline 
void i2c_wait(void) {
	while(((I2C0->S & I2C_S_IICIF_MASK)==0) & (lock_detect < 200)) {
		lock_detect++;
	} 
	if (lock_detect >= 200)
		i2c_busy();
	
  I2C0->S |= I2C_S_IICIF_MASK;
}
#else
#pragma no_inline 
void i2c_wait(void) {
	while((I2C0->S & I2C_S_IICIF_MASK)==0) {
		} 
  I2C0->S |= I2C_S_IICIF_MASK;
}
#endif

//send start sequence
void i2c_start()
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}

//send device and register addresses
#pragma no_inline 
void i2c_read_setup(uint8_t dev, uint8_t address)
{
  I2C0->D = dev;        /*send dev address  */
  I2C_WAIT              /*wait for completion */
  
  I2C0->D =  address;    /*send read address  */
  I2C_WAIT              /*wait for completion */
    
  I2C_M_RSTART;           /*repeated start */
  I2C0->D = (dev|0x1);   /*send dev address (read)  */
  I2C_WAIT               /*wait for completion */
  
  I2C_REC;               /*set to receive mode */

}

//read a byte and ack/nack as appropriate
// #pragma no_inline 
uint8_t i2c_repeated_read(uint8_t isLastRead)
{
  uint8_t data;
  
  lock_detect = 0;
  
  if(isLastRead)  {
    NACK;                /*set NACK after read  */
  } else  {
    ACK;                /*ACK after read  */
  }
  
  data = I2C0->D;        /*dummy read  */
  I2C_WAIT              /*wait for completion */
  
  if(isLastRead)  {
    I2C_M_STOP;          /*send stop  */
  }
  data = I2C0->D;        /*read data  */

  return  data;          
}

//////////funcs for reading and writing a single byte
//using 7bit addressing reads a byte from dev:address
// #pragma no_inline 
uint8_t i2c_read_byte(uint8_t dev, uint8_t address)
{
	uint8_t data;
	
	I2C_TRAN;	/*set to transmit mode */
	I2C_M_START;	/*send start	*/
	I2C0->D = dev;	/*send dev address	*/
	I2C_WAIT		/*wait for completion */
	
	I2C0->D =  address;	/*send read address	*/
	I2C_WAIT	/*wait for completion */
		
	I2C_M_RSTART;	/*repeated start */
	I2C0->D = (dev|0x1);	/*send dev address (read)	*/
	I2C_WAIT	/*wait for completion */
	
	I2C_REC;	/*set to receive mode */
	NACK;	/*set NACK after read	*/
	
	data = I2C0->D;	/*dummy read	*/
	I2C_WAIT	/*wait for completion */
	
	I2C_M_STOP;	/*send stop	*/
	data = I2C0->D;	/*read data	*/

	return data;
}



//using 7bit addressing writes a byte data to dev:address
#pragma no_inline 
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data)
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
	I2C0->D = dev;			  /*send dev address	*/
	I2C_WAIT						  /*wait for ack */
	
	I2C0->D =  address;		/*send write address	*/
	I2C_WAIT
		
	I2C0->D = data;				/*send data	*/
	I2C_WAIT
	I2C_M_STOP;
}
