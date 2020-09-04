#ifndef __X_UTIL_COMMON_H__
#define __X_UTIL_COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <stdint.h>


#define X_MALLOC(s)      malloc(s)
#define X_CALLOC(n,s)      calloc(n, s)
#define X_FREE(p)        if(p) {free(p); p = NULL;}
#define X_REALLOC(p, s)  realloc(p, s)

#define X_MEMSET(p,v,s)   memset(p,v,s)
#define X_MEMCPY(p,v,s)  memcpy(p,v,s)

#define IS_POW_OF_2(x) !((x) & ((x) - 1))


static inline uint32_t next_pow_of_2(uint32_t x){
    if(IS_POW_OF_2(x)) return x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

#endif
