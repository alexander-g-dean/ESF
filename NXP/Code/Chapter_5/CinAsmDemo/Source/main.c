/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>

extern int fun4(char a, int b, char c);
extern void arrays(unsigned char n, unsigned char j);
extern void static_auto_local( void );
extern int fun5(char a, int b, int c, char d);

int main(void)
{
	volatile int n;
		arrays(2, 4);
		fun4(1,2000,3);
		static_auto_local();
		n = fun5(1,2,3,4);
		while (1)
			;
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
