#ifndef FROGGER_RING_BUFFER_H
#define FROGGER_RING_BUFFER_H

#include <stdint.h>

#define RING_BUFFER_SIZE 32

typedef struct {
    int data[RING_BUFFER_SIZE];
    int head;
    int count;
} IntRingBuffer;

void ring_init(IntRingBuffer* rb);
void ring_push(IntRingBuffer* rb, int val);
int ring_at(const IntRingBuffer* rb, int idx);
int ring_count(const IntRingBuffer* rb);

#endif /* FROGGER_RING_BUFFER_H */
