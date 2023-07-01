#include "libmuzzy/vec.h"
#include "libmuzzy/macros.h"
#include <stdlib.h>
#include <string.h>

struct muzzy_vec muzzy_vec_init(size_t stride) {
  struct muzzy_vec self;
  memclear(self);

  self.stride = stride;
  self.max_len = MUZZY_VEC_DEFAULT_MAX_LEN;
  self.data = malloc(self.stride * self.max_len);
  memset(self.data, 0, self.stride * self.max_len);

  return self;
}

void muzzy_vec_resize(struct muzzy_vec *self) {}

void muzzy_vec_push(struct muzzy_vec *self, void *item) {}
void *muzzy_vec_pop(struct muzzy_vec *self, void *item) {}

void *muzzy_vec_get(struct muzzy_vec *self, size_t index) {}

void muzz_vec_free(struct muzzy_vec *self) { free(self->data); }
