#include "x_msgq.h"
#include <stdio.h>

int main(int argc, char *argv[]){
    int fd = x_msgq_open("/msgq_test", 0);
    char data[4] = {0};
    int i = 0;
    
    while(i++ < 300){
        printf("msgq send result:%d\n",x_msgq_send(fd, data, sizeof(data),i % 4, 500));
    }
    x_msgq_close(fd);
}
