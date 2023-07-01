#ifndef VEC_H_
#define VEC_H_

#include <stddef.h>

#define muzzy_vec_pop(type, self) ((type)) muzzy_vec_pop((self))
#define muzzy_vec_get(type, self, index) ((type)) muzzy_vec_get((self), (index))

struct muzzy_vec {
  size_t stride;
  size_t max_len;
  size_t len;
  void *data;
};

struct muzzy_vec muzzy_vec_init(size_t stride);

void muzzy_vec_push(struct muzzy_vec *self, void *item);
void *muzzy_vec_pop_vp(struct muzzy_vec *self, void *item);

void *muzzy_vec_get_vp(struct muzzy_vec *self, size_t index);

void muzz_vec_free(struct muzzy_vec *self);

#endif
