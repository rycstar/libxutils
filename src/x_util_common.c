#include "x_util_common.h"

#define NANOSEC_2_SEC (1000*1000*1000)
struct timespec * timeoutCalc(int timeout, struct timespec * timespec_, struct timespec * now){
    if(timeout > 0 && timespec_){
        timespec_->tv_sec = timeout / 1000; 
        timespec_->tv_nsec = (1000 * 1000) * (timeout % 1000);
        if(now){
            timespec_->tv_sec += now->tv_sec;
            timespec_->tv_nsec += now->tv_nsec;
            if(timespec_->tv_nsec >= NANOSEC_2_SEC){
                timespec_->tv_sec ++;
                timespec_->tv_nsec -= NANOSEC_2_SEC;
            }
        }
        return timespec_;
    }
    return NULL;
}


