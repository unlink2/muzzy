#ifndef VEC_H_
#define VEC_H_

#include <stddef.h>

#define MUZZY_VEC_DEFAULT_MAX_LEN 16

struct muzzy_vec {
  size_t stride;
  size_t max_len;
  size_t len;
  void *data;
};

struct muzzy_vec muzzy_vec_init(size_t stride);

void muzzy_vec_push(struct muzzy_vec *self, void *item);
void *muzzy_vec_pop(struct muzzy_vec *self);

void *muzzy_vec_get(struct muzzy_vec *self, size_t index);

void muzzy_vec_free(struct muzzy_vec *self);

#endif
