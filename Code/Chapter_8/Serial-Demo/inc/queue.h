#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <MKL25Z4.h>

#define Q_MAX_SIZE (256)

typedef struct {
  unsigned int Head; // Index of oldest data element 
  unsigned int Tail; // Index of next free space
  unsigned int Size; // Number of elements in use
  uint8_t Data[Q_MAX_SIZE];
} volatile Q_T;


extern int Q_Empty(Q_T * q);
extern int Q_Full(Q_T * q);
extern int Q_Size(Q_T * q);
extern int Q_Enqueue(Q_T * q, uint8_t d);
extern uint8_t Q_Dequeue(Q_T * q);
extern void Q_Init(Q_T * q);

#endif // QUEUE_H
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
