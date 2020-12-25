#include <stdio.h>
#include "x_shmem.h"

int main(int argc, char * argv[]){
    int fd = 0, i = 0;
    char * buf = x_shmem_open("/shm_test", 1, 8, &fd);
    char w_buf[8] = {0};
    if(!buf){
        printf("-----share memory open error------\n");
        return -1;
    }
    memset(w_buf, 0 ,sizeof(w_buf));   
    while(1){
        memset(w_buf, 0x30 + ((i++)%10), 7);
        w_buf[7] = 0;
        x_shmem_write(buf,0,w_buf,sizeof(w_buf));
        usleep(5000*1000);
    }

    x_shmem_close(fd, &buf);

    return 0;
}

