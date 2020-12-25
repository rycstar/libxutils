#include <stdio.h>
#include "x_shmem.h"

int main(int argc, char * argv[]){

    int fd = 0;
    char buf[8]={0};
    char * s_buf = (char *)x_shmem_open("/shm_test", 0, 8, &fd);

    if(!s_buf){
        printf("------------open shm error---\n");
        return -1;
    }
    while(1){
        x_shmem_lock(s_buf);
        usleep(200*1000);
        snprintf(buf, sizeof(buf), "%s",s_buf);
        x_shmem_unlock(s_buf);

        printf("shmem_read get string:%s\n",buf);
        usleep(100*1000);
    }   

    return 0;
}
