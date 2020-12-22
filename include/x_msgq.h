#ifndef __X_MSGQ_H__
#define __X_MSGQ_H__

#include <stddef.h>

#define MSGQ_OPEN_MODE_WRITE (0x01 << 0)
#define MSGQ_OPEN_MODE_PRIV (0x01 << 1)

int x_msgq_open(const char* name, int flag);

int x_msgq_send(int msgq_id, const char * data, size_t data_len, unsigned int priority,int timeout/*ms*/);

/*
* always recv the highest priority msg first
*/
int x_msgq_recv(int msgq_id, char * data, size_t data_len,int timeout/*ms*/);

int x_msgq_close(int fd);

int x_msgq_delete(const char* name);

#endif

