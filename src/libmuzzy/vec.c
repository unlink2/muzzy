#include "libmuzzy/vec.h"
#include "libmuzzy/error.h"
#include "libmuzzy/macros.h"
#include "libmuzzy/log.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct muzzy_vec muzzy_vec_init(size_t stride) {
  struct muzzy_vec self;
  memset(&self, 0, sizeof(self));

  self.stride = stride;
  self.max_len = MUZZY_VEC_DEFAULT_MAX_LEN;
  self.data = malloc(self.stride * self.max_len);
  memset(self.data, 0, self.stride * self.max_len);

  return self;
}

#define muzzy_vec_offset(self, index)                                          \
  ((uint8_t *)(self)->data + (self)->stride * (index))

void muzzy_vec_resize(struct muzzy_vec *self) {
  self->max_len = self->max_len * 2;
  void *new_data = realloc(self->data, self->stride * self->max_len);

  if (new_data) {
    self->data = new_data;
  } else {
    muzzy_errno();
  }
}

void muzzy_vec_push(struct muzzy_vec *self, void *item) {
  if (self->len >= self->max_len) {
    muzzy_vec_resize(self);
    if (muzzy_err()) {
      muzzy_error("Unable to resize vec %p!\n", (void *)self);
      return;
    }
  }

  memcpy(muzzy_vec_offset(self, self->len), item, self->stride);
  self->len++;
}

void *muzzy_vec_pop(struct muzzy_vec *self) {
  void *d = muzzy_vec_get(self, self->len - 1);
  if (d) {
    self->len--;
  }
  return d;
}

void *muzzy_vec_get(struct muzzy_vec *self, size_t index) {
  if (self->len <= index) {
    return NULL;
  }
  return muzzy_vec_offset(self, index);
}

void muzzy_vec_clear(struct muzzy_vec *self) { self->len = 0; }

void muzzy_vec_free(struct muzzy_vec *self) { free(self->data); }
