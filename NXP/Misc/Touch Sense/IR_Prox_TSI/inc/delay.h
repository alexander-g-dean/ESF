#ifndef DELAY_H
#define DELAY_H
extern void Delay(uint32_t dlyTicks);
extern void SysTick_Init(void);
extern void SysTick_Wait(uint16_t delay);
extern void SysTick_Wait1ms(uint16_t delay);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
