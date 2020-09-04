
#include "x_vector.h"
#include "x_util_common.h"

void x_vector_init(xVector *vector, int cap, int data_size){
    // initialize size and capacity
    vector->size = 0;
    
    vector->capacity = next_pow_of_2(cap);

    vector->data_size = data_size;
    // allocate memory for vector->data
    vector->data = (void *)X_MALLOC((vector->data_size) * vector->capacity);
}

void x_vector_append(xVector *vector, void * value){
    // make sure there's room to expand into
    x_vector_double_capacity_if_full(vector);

    // append the value and increment vector->size
    if(value){
        X_MEMCPY((char*)vector->data + (vector->size * vector->data_size),value,vector->data_size);
    }else{
        X_MEMSET((char*)vector->data + (vector->size * vector->data_size),0,vector->data_size);
    }
    vector->size++;
}

void x_vector_remove(xVector *vector, int index){
    char *des = NULL, *src = NULL;
    int size;

    if(index >= vector->size || index < 0) {
        printf("%s: Index %d out of bounds for vector of size %d\n", __FUNCTION__, index, vector->size);
        return;
    }
    des = ((char*)vector->data) + (index * vector->data_size);
    src = des + vector->data_size;
    size = vector->size - index - 1;
    X_MEMCPY(des, src, size * vector->data_size);
    vector->size--;
}

void x_vector_clear(xVector *vector){
    /*just clear the number of slot used,don't free the data*/
    if(vector) vector->size = 0;
}


void * x_vector_get(xVector *vector, int index){
    if (index >= vector->size || index < 0) {
        printf("%s: Index %d out of bounds for vector of size %d\n", __FUNCTION__, index, vector->size);
        return 0;
    }
    return (void *)(((char*)vector->data) + (index* vector->data_size));
}

void x_vector_set(xVector *vector, int index, void * value){
      // zero fill the vector up to the desired index
      while (index >= vector->size) {
        x_vector_append(vector, 0);
      }
    
      // set the value at the desired index
      if(value){
          X_MEMCPY((char*)vector->data + (vector->size * vector->data_size),value,vector->data_size);
      }else{
          X_MEMSET((char*)vector->data + (vector->size * vector->data_size),0,vector->data_size);
      }

}

void x_vector_double_capacity_if_full(xVector *vector){
    if (vector->size >= vector->capacity) {
        // double vector->capacity and resize the allocated memory accordingly
        vector->capacity *= 2;
        vector->data = X_REALLOC(vector->data, (vector->data_size) * vector->capacity);
    }
}

void x_vector_free(xVector *vector){
    X_FREE(vector->data);
}

