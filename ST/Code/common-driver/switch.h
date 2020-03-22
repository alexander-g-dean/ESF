#ifndef SWITCH_H
#define SWITCH_H

// Start Listing 3.2a
#define SW_GPIO (GPIOC)	// Both switches are on GPIO C
#define SW1_POS (13) 		// PC13 (User Button B1)
#define SW2_POS (7)  		// PC7 (add external button)

#define MASK(x) (1UL << (x))

// Both switches are active-low, so invert data
#define SWITCH_PRESSED(x) (!((SW_GPIO)->IDR & MASK(x)))

void Init_GPIO_Switches(void);

void Init_GPIO_Switches_Interrupts(void);
// End Listing 3.2a


#endif // SWITCH_H
