#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

// Start Listing Q_Defs

#define Q_MAX_SIZE (256)
typedef struct {
    uint8_t Data[Q_MAX_SIZE];
    // Index of oldest data element
    unsigned int Head;
    // Index of next free space
    unsigned int Tail;
    // Number of elements in use
    unsigned int Size;
} Q_T;
// End Listing Q_Defs

int Q_Enqueue(Q_T *q, uint8_t d);

uint8_t Q_Dequeue(Q_T *q);

void Q_Init(Q_T *q);

int Q_Empty(Q_T *q);

int Q_Full(Q_T * q);

int Q_Size(Q_T *q);

#endif

