#ifndef __X_SHMEM_H__
#define __X_SHMEM_H__
#include <stdatomic.h>
#include "x_util_common.h"

void * x_shmem_open(const char * name, int flag, size_t sz, int * s_fd);

int    x_shmem_lock(void * m_buf);

int    x_shmem_unlock(void * m_buf, int wake_flag);

int    x_shmem_write(void * m_buf, size_t offset, void * w_buf,size_t w_size);

int    x_shmem_read(void * m_buf, size_t offset,void * r_buf,size_t r_size);


uint32_t    x_shmem_get_serial(void * m_buf);

int    x_shmem_listen(void * m_buf, uint32_t old_serial, int timeout);

void   x_shmem_close(int fd, void ** m_buf);

int   x_shmem_delete(const char * name);

#endif

