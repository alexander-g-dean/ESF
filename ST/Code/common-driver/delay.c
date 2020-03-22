#include "delay.h"

// Start Listing 2.Delay
void Delay(volatile unsigned int time_del) {
	volatile int n;
	while (time_del--) {
		n = 1000;
		while (n--)
			;
	}
}
// End Listing 2.Delay


// Start Listing 2.ShortDelay
void ShortDelay(volatile unsigned int time_del) {
	volatile int n;

	while (time_del--);
}

// End Listing 2.ShortDelay
