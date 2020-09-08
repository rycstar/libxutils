#ifndef __X_UTIL_COMMON_H__
#define __X_UTIL_COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/syscall.h>

#define X_MALLOC(s)      malloc(s)
#define X_CALLOC(n,s)      calloc(n, s)
#define X_FREE(p)        if(p) {free(p); p = NULL;}
#define X_REALLOC(p, s)  realloc(p, s)

#define X_MEMSET(p,v,s)   memset(p,v,s)
#define X_MEMCPY(p,v,s)  memcpy(p,v,s)

#define IS_POW_OF_2(x) !((x) & ((x) - 1))

#define x_offsetof(TYPE, MEMBER) ((size_t)&((TYPE*)0)->MEMBER)

#define x_container_of(ptr, type, member) ({ \
        const typeof(((type*)0)->member) *__ptr = (ptr); \
        (type *)((char*)__ptr - x_offsetof(type, member)); \
})

static inline uint32_t next_pow_of_2(uint32_t x){
    if(IS_POW_OF_2(x)) return x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}


static inline int sys_futex(void *addr1, int op, int val1, struct timespec *timeout, void *addr2, int val3)
{
	int saved_errno = errno;
	int res = syscall(__NR_futex, addr1, op, val1, timeout, addr2, val3);
	if(res == -1){
		res = -errno;
		errno = saved_errno;
	}
	return res;
}

#endif
