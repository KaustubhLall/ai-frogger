#include "core/ring_buffer.h"

void ring_init(IntRingBuffer* rb) {
    rb->head = 0;
    rb->count = 0;
}

void ring_push(IntRingBuffer* rb, int val) {
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->data[rb->head] = val;
    if (rb->count < RING_BUFFER_SIZE) rb->count++;
}

int ring_at(const IntRingBuffer* rb, int idx) {
    if (idx < 0 || idx >= rb->count) return -1;
    int pos = (rb->head - rb->count + 1 + idx + RING_BUFFER_SIZE) % RING_BUFFER_SIZE;
    return rb->data[pos];
}

int ring_count(const IntRingBuffer* rb) {
    return rb->count;
}
