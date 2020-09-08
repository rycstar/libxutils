#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include "x_shmem.h"

#define SERIAL_LOCK_VAL (0x01u)

#define X_SHMEM_SERIAL_LOCKED(serial) ((serial) & SERIAL_LOCK_VAL)
#define X_SHMEM_SERIAL_UNLOCKED(serial) (!(X_SHMEM_SERIAL_LOCKED(serial)))

#define X_SHMEM_SERIAL_NO_UPDATE(serial, old_serial) (!((serial^old_serial) & 0xFFFF0000u))


typedef struct _shmem_data{
    atomic_uint serial;/*For data sync in processes*/
    size_t data_len;
    char data[0];
}__attribute__((packed))tShmemData;


static struct timespec * timeoutCalc(int timeout, struct timespec * timespec_){
	if(timeout > 0 && timespec_){
		timespec_->tv_sec = timeout / 1000; 
		timespec_->tv_nsec = (1000 * 1000) * (timeout % 1000);
		return timespec_;
	}
	return NULL;
}


/*
* get a region of shmem  (use posix api).
* Flag: read or write (included in fcntl.h)
*/
void * x_shmem_open(const char * name, int flag, size_t sz, int * s_fd){
    int fd = 0;
    struct stat f_st;
    void * memory_area = 0;
    /*we need to write serial to sync data, so must be set the O_RDWR for each process*/
    int o_flag = O_RDWR | O_CLOEXEC, m_flag = PROT_READ | PROT_WRITE;
    tShmemData * sd = NULL;
    size_t map_size = sizeof(tShmemData) + sz;

    if(flag){
        o_flag |= (O_CREAT);
    }
    
    fd = shm_open(name, o_flag, 0666);

    if(fd <= 0) return NULL;
    fstat(fd, &f_st);

    if(f_st.st_size == 0){
        if(0 > ftruncate(fd, map_size)) goto map_fail;
    }else if(f_st.st_size != map_size){
        printf("Error: request memory size(%d) is different from the fd size(%ld)\n",sz,f_st.st_size);
        goto map_fail;
    }

    memory_area = mmap(NULL,f_st.st_size, m_flag,MAP_SHARED, fd, 0);

    if(memory_area == MAP_FAILED){
        printf("Error: Mmap failed from file : %s, size : %ld\n",name,f_st.st_size);
        goto map_fail;
    }

    sd = (tShmemData *)memory_area;
    if(flag){
        atomic_init(&sd->serial, 0u);
        sd->data_len = sz;
        memset(sd->data, 0, sz);
    }
    if(s_fd) *s_fd = fd;
    return (void *)sd->data;

map_fail:
    close(fd);
    return NULL;
}


int    x_shmem_lock(void * m_buf){
    uint32_t serial = 0;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);

    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    while(X_SHMEM_SERIAL_LOCKED(serial)){
        sys_futex(&sd->serial, FUTEX_WAIT,serial,NULL,NULL,0);
        serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    }
    serial |= SERIAL_LOCK_VAL;
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    return 0;
}

int    x_shmem_unlock(void * m_buf){
    uint32_t serial = 0;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);

    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    if(X_SHMEM_SERIAL_UNLOCKED(serial)){
        /*is unlock state, return*/
        return 0;
    }
    serial &= (~SERIAL_LOCK_VAL);
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    sys_futex(&sd->serial, FUTEX_WAKE,INT_MAX,NULL,NULL,0);
    return 0;
}


int    x_shmem_write(void * m_buf, size_t offset, void * w_buf,size_t w_size){
    uint32_t serial = 0;
    uint32_t write_count = 0;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);
    
    if((w_size + offset) > sd->data_len || !w_buf) return -1;

    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    while(X_SHMEM_SERIAL_LOCKED(serial)){
        sys_futex(&sd->serial, FUTEX_WAIT,serial,NULL,NULL,0);
        serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    }
    serial |= SERIAL_LOCK_VAL;
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    memcpy(sd->data + offset , w_buf, w_size);
    write_count = (serial >> 16) & 0x0000FFFFu;
    serial = ((++write_count) << 16) & 0xFFFF0000u;
    serial &= (~SERIAL_LOCK_VAL);
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    sys_futex(&sd->serial, FUTEX_WAKE,INT_MAX,NULL,NULL,0);
    return 0;

}

int    x_shmem_read(void * m_buf, size_t offset, void * r_buf,size_t r_size){
    uint32_t serial = 0;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);
    if((r_size + offset) > sd->data_len || !r_buf){
        return -1;
    }
    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    while(X_SHMEM_SERIAL_LOCKED(serial)){
        sys_futex(&sd->serial, FUTEX_WAIT,serial,NULL,NULL,0);
        serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    }
    serial |= SERIAL_LOCK_VAL;
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    memcpy(r_buf, sd->data + offset, r_size);
    serial &= ~(SERIAL_LOCK_VAL); 
    atomic_store_explicit(&sd->serial, serial, memory_order_release);
    sys_futex(&sd->serial, FUTEX_WAKE,INT_MAX,NULL,NULL,0);
    return 0;
}

uint32_t    x_shmem_get_serial(void * m_buf){
    uint32_t serial = 0;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);
    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);
    return serial;
}


int     x_shmem_listen(void * m_buf,uint32_t old_serial, int timeout){
    int rc = 0;
    uint32_t serial = 0, tmp_serial = old_serial;
    struct timespec ts;
    tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);

    serial = atomic_load_explicit(&sd->serial, memory_order_acquire);

    do{
		if((rc = sys_futex(&sd->serial, FUTEX_WAIT,tmp_serial,timeoutCalc(timeout,&ts),NULL,0)) != 0 && rc != -ETIMEDOUT){
			return -1;
		}
		tmp_serial = atomic_load_explicit(&sd->serial,memory_order_acquire);
	}while(X_SHMEM_SERIAL_NO_UPDATE(serial,tmp_serial));
    return 0;
}


void   x_shmem_close(int fd, void ** m_buf){
    size_t m_size = 0;
    if(*m_buf){
        tShmemData *sd = x_container_of((const char *)m_buf, tShmemData, data);
        m_size = sizeof(tShmemData) + sd->data_len;
        munmap(sd, m_size);
        *m_buf = NULL;
        if(fd > 0) close(fd);
    }
}

int   x_shmem_delete(const char * name){
    return shm_unlink(name);
}


