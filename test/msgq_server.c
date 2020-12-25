#include "x_msgq.h"
#include <stdio.h>

int main(int argc, char * argv[]){
    int fd = x_msgq_open("/msgq_test",1);

    while(1){
        usleep(1000*1000);
    }
}
