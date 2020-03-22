#ifndef QUEUE_H
#define QUEUE_H

#define Q_SIZE (256)

typedef struct {
  unsigned short Data[Q_SIZE];
  unsigned int Head; // points to oldest data element 
  unsigned int Tail; // points to next free space
  unsigned int Size; // quantity of elements in queue
} Q_T;


extern int Q_Empty(Q_T * q);
extern int Q_Full(Q_T * q);
extern int Q_Enqueue(Q_T * q, unsigned short d);
extern unsigned short Q_Dequeue(Q_T * q);
extern void Q_Init(Q_T * q);

#endif // QUEUE_H
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
