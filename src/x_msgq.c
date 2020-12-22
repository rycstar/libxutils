#include <mqueue.h>
#include <time.h>
#include "x_util_common.h"
#include "x_msgq.h"

/*
* why choose POSIX msgq ?
* 1. POSIX MsgQ has thread-safe APIs (System V msgq is not thread-safe)
* 2. POSIX MsgQ has msg priority, (PIPE can not)
* 3. POSIX MsgQ can use poll/select to get event
* 4. POSIX MsgQ use filename as identify flag.(System V msgq use a int key which we can't make sure is different from others.
*    ftok function can translate a string to a key, but can't make sure that the key is a unique one)
* Notice: POSIX MsgQ will create file in directory /dev/mqueue/,
* So we need to check the directory exist or not.
*
* In embeded system, user can modify msgq limit through /proc/sys/fs/mqueue/msg_max and etc...
* I think msg_max = 64, msgsize_max= 1280 should be better.

*Disadvantage: Posix message queue don't support private mode, so sad...
*For private mode, you should use System V message queue
*
*/

#define POSIX_MSGQ_PATH "/dev/mqueue"

int x_msgq_open(const char* name, int flag){
    int msgq_id = 0;
    int o_flag = O_RDWR | O_CLOEXEC;
    if(access(POSIX_MSGQ_PATH,F_OK) < 0) {
        printf("Error: directory <%s> not exist,please create it!\n",POSIX_MSGQ_PATH);
        return -1;
    }
    if(flag & MSGQ_OPEN_MODE_WRITE) o_flag |= O_CREAT;
    
    msgq_id = mq_open(name, o_flag, 0666, NULL);
    if(msgq_id <= 0){
        printf("Error: msg queue open fail. path:<%s>!\n",name);
        return -1;
    }

    return msgq_id;
}

int x_msgq_send(int msgq_id, const char * data, size_t data_len, unsigned int priority,int timeout/*ms*/){
    struct timespec ts, now_time;
    struct timespec * tmp_ts = NULL;

    if(timeout > 0){
        clock_gettime(CLOCK_REALTIME, &now_time);
        tmp_ts = timeoutCalc(timeout, &ts,&now_time);
    }
    if(!tmp_ts){
        return mq_send(msgq_id,data,data_len,priority);
    }else{
        return mq_timedsend(msgq_id,data,data_len,priority,tmp_ts);
    }
}

/*
* always recv the highest priority msg first
*/
int x_msgq_recv(int msgq_id, char * data, size_t data_len,int timeout/*ms*/){
    unsigned int priority = 0;
    struct timespec ts, now_time;
    struct timespec * tmp_ts = NULL;

    if(timeout > 0){
        clock_gettime(CLOCK_REALTIME, &now_time);
        tmp_ts = timeoutCalc(timeout, &ts,&now_time);
    }

    if(!tmp_ts){
        return mq_receive(msgq_id,data,data_len,&priority);
    }else{
        return mq_timedreceive(msgq_id,data,data_len,&priority,tmp_ts);
    }
}


int x_msgq_close(int fd){
    if(fd > 0) mq_close(fd);
    return 0;
}

int x_msgq_delete(const char* name){
    if(name) mq_unlink(name);
    return 0;
}


