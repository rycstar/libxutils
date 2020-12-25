#include "x_ringbuffer.h"
#include <pthread.h>
#include <stdio.h>

void * r_ringbuffer_run(void *arg){
    tRingBuffer * fifo = (tRingBuffer *)arg;
    char r_buf[48] = {0};
    int r_len = 0;
    printf("ringbuffer size:%d\n", fifo->size);
    while(1){
        if(x_ring_buffer_outlen(fifo) >= 40){
            r_len = x_ring_buffer_read(fifo, r_buf, 40);
            r_buf[r_len] = 0;
            printf("read string:%s len:%d\n", r_buf, r_len);
        }
        usleep(1000*1000);
    }

    return NULL;
}

void * w_ringbuffer_run(void *arg){
    tRingBuffer * fifo = (tRingBuffer *)arg;
    char w_buf[48] = {0};
    int w_len = 0,i = 0;
    while(1){
        if(x_ring_buffer_inlen(fifo) >= 32){
            memset(w_buf, 0, 48);
            memset(w_buf, 0x30+(i++)%0x10, 24);
            w_len = x_ring_buffer_write(fifo, w_buf, 24);
            
            printf("write string:%s len:%d\n", w_buf, w_len);
        }
        usleep(800*1000);
    }

    return NULL;
}

int main(int argc, char * argv[]){
    pthread_t r_id, w_id;
    tRingBuffer * ring_buf = x_ring_buffer_create(1023);
    pthread_create(&r_id, NULL,r_ringbuffer_run,(void*)ring_buf);
    pthread_create(&w_id, NULL,w_ringbuffer_run,(void*)ring_buf);

    pthread_join(r_id,NULL);
    pthread_join(w_id,NULL);
}
