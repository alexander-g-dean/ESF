#include <stdint.h>
#include <cmsis_armcc.h>
#include "queue.h"

/* Listing 8.2 */
int Q_Enqueue(Q_T *q, uint8_t d) {
    uint32_t masking_state;
    // If queue is full, don't overwrite data, but do return an error code
    if (!Q_Full(q)) {
        q->Data[q->Tail++] = d;
        q->Tail %= Q_MAX_SIZE;
        // Protect q->Size++ operation from preemption
        // Save current masking state
        masking_state = __get_PRIMASK();
        // Disable interrupts
        __disable_irq();
        // Update variable
        q->Size++;
        // Restore interrupt masking state
        __set_PRIMASK(masking_state);
        return 1; // Success
    } else
        return 0; // Failure
}

uint8_t Q_Dequeue(Q_T *q) {
    uint32_t masking_state;
    uint8_t t = 0;
    // Check to see if queue is empty before dequeuing
    if (!Q_Empty(q)) {
        t = q->Data[q->Head];
        q->Data[q->Head++] = '_'; // Empty unused entries for debugging
        q->Head %= Q_MAX_SIZE;
        // Protect q->Size-- operation from preemption
        // Save current masking state
        masking_state = __get_PRIMASK();
        // Disable interrupts
        __disable_irq();
        // Update variable
        q->Size--;
        // Restore interrupts
        __set_PRIMASK(masking_state);
    }
    return t;
}

/* Listing 8.3 */
void Q_Init(Q_T *q) {
    unsigned int i;
    for (i = 0; i < Q_MAX_SIZE; i++)
        // To simplify our lives when debugging
        q->Data[i] = 0;
    q->Head = 0;
    q->Tail = 0;
    q->Size = 0;
}

int Q_Empty(Q_T *q) {
    return q->Size == 0;
}

int Q_Full(Q_T * q) {
    return q->Size == Q_MAX_SIZE;
}

int Q_Size(Q_T *q) {
    return q->Size;
}
