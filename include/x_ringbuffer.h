#ifndef __X_RINGBUFFERH__
#define __X_RINGBUFFERH__
#include <stdint.h>

typedef struct{
    char * buffer;
    uint32_t size;
    uint32_t in;
    uint32_t out;
}tRingBuffer;

static inline void x_ring_buffer_reset(tRingBuffer *fifo)
{
  fifo->in = fifo->out = 0;
}

static inline uint32_t x_ring_buffer_size(tRingBuffer *fifo)
{
  return fifo->size;
}

static inline uint32_t x_ring_buffer_outlen(tRingBuffer *fifo)
{
  return fifo->in - fifo->out;
}

static inline uint32_t x_ring_buffer_inlen(tRingBuffer *fifo)
{
  return x_ring_buffer_size(fifo) - x_ring_buffer_outlen(fifo);
}


static inline int x_ring_buffer_empty(tRingBuffer *fifo)
{
  return x_ring_buffer_outlen(fifo) == 0;
}


static inline int x_ring_buffer_full(tRingBuffer *fifo)
{
  return x_ring_buffer_inlen(fifo) == 0;
}


tRingBuffer*    x_ring_buffer_create(uint32_t size);
void            x_ring_buffer_destroy(tRingBuffer *fifo);

uint32_t        x_ring_buffer_write(tRingBuffer *fifo, char * buf, uint32_t len);
uint32_t        x_ring_buffer_read(tRingBuffer *fifo, char * buf, uint32_t len);

#endif

