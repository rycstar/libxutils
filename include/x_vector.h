#ifndef __X_VECTOR_H__
#define __X_VECTOR_H__

#define DEFAULT_VECTOR_INITIAL_CAPACITY 4

// Define a vector type
typedef struct {
  int size;      /*slots used so far*/
  int capacity;  /* total available slots*/
  void *data;     /*array of element we're storing*/
  int data_size; /*the element unit size,should not be changed after vector inited*/
} xVector;

void x_vector_init(xVector *vector, int cap, int data_size);

void x_vector_append(xVector *vector, void * value);

void x_vector_remove(xVector * vector, int index);

void x_vector_clear(xVector *vector);

void * x_vector_get(xVector *vector, int index);

void x_vector_set(xVector *vector, int index, void * value);

void x_vector_double_capacity_if_full(xVector *vector);

void x_vector_free(xVector *vector);

#endif
