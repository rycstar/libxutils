#include <stdio.h>
#include "x_shmem.h"

int main(int argc, char * argv[]){
    int fd = 0, i = 0;
    uint32_t tmp_serial = 0;
    char r_buf[8] = {0};
    char * buf = x_shmem_open("/shm_test", 0, 8, &fd);

    if(!buf) return -1;
    while(i++ < 100){
        tmp_serial = x_shmem_get_serial(buf);
        if(x_shmem_listen(buf,tmp_serial,2000) >= 0){
            x_shmem_read(buf, 4, r_buf, sizeof(r_buf) - 4);   
            printf("x_shmem listen (%d), get string:%s\n",i,r_buf);
        }else{
            printf("x_shmem listrn timeout error...\n");
        }
    }
    x_shmem_close(fd, &buf);
    return 0;
}
