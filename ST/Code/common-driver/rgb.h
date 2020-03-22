#ifndef RGB_H
#define RGB_H

#define LED_R   GPIO_PIN_5 //PA5
#define LED_G   GPIO_PIN_6 //PA6
#define LED_B   GPIO_PIN_7 //PA7

// Active-Low LED control definitions
// 0 out = LED on
// 1 out = LED off
#define LED_R_OFF_MSK	(GPIO_BSRR_BS_5)	
#define LED_R_ON_MSK	(GPIO_BSRR_BR_5)
#define LED_G_OFF_MSK	(GPIO_BSRR_BS_6)
#define LED_G_ON_MSK	(GPIO_BSRR_BR_6)
#define LED_B_OFF_MSK	(GPIO_BSRR_BS_7)
#define LED_B_ON_MSK	(GPIO_BSRR_BR_7)

void Init_GPIO_RGB(void);

void Control_RGB_LEDs(int r_on, int g_on, int b_on);
void Control_RGB_LEDs_2(int r_on, int g_on, int b_on);

#endif
