#ifndef DMA_H
#define DMA_H

extern void Test_DMA_Copy(void);

void Init_DMA_For_Playback(uint16_t * source, uint32_t count);
void Start_DMA_Playback(void);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
