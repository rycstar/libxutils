#include "x_ringbuffer.h"
#include "x_util_common.h"

tRingBuffer*    x_ring_buffer_create(uint32_t size){
    tRingBuffer*  rb = NULL;
    uint32_t buf_size = size;

    if(!IS_POW_OF_2(size)){
        buf_size = next_pow_of_2(size);
    }
    
    rb = (tRingBuffer*)X_MALLOC(sizeof(tRingBuffer));
    if(!rb) return rb;
    memset(rb, 0, sizeof(tRingBuffer));
    rb->size = buf_size;
    rb->buffer = (char *)X_MALLOC(rb->size);
    if(!rb->buffer){
        X_FREE(rb);
    }
    
    return rb;
}

void            x_ring_buffer_destroy(tRingBuffer *fifo){
    if(fifo){
        X_FREE(fifo->buffer);
        X_FREE(fifo);
    }
}

uint32_t        x_ring_buffer_write(tRingBuffer *fifo, char * buf, uint32_t len){
    uint32_t w_len = X_MIN(len,x_ring_buffer_inlen(fifo));
    uint32_t l = 0;

    l = X_MIN(w_len, fifo->size - (fifo->in & (fifo->size - 1)));
    X_MEMCPY(fifo->buffer + (fifo->in & (fifo->size - 1)), buf, l);

    /* Then put the rest (if any) at the beginning of the buffer. */
    X_MEMCPY(fifo->buffer, buf + l, w_len - l);

    fifo->in += w_len;
    return w_len;
}

uint32_t        x_ring_buffer_read(tRingBuffer *fifo, char * buf, uint32_t len){
    uint32_t r_len = X_MIN(len,x_ring_buffer_outlen(fifo));
    uint32_t l = 0;

    l = X_MIN(r_len, fifo->size - (fifo->out & (fifo->size - 1)));

    X_MEMCPY(buf,fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    /* Then get the rest (if any) at the beginning of the buffer. */
    X_MEMCPY(buf + l, fifo->buffer, r_len - l);

    fifo->out += r_len;
    return r_len;
}


