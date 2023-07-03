#include "libmuzzy/buffer.h"
#include "libmuzzy/error.h"
#include "libmuzzy/log.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_buffer muzzy_buffer_init(void) {
  struct muzzy_buffer self;
  memset(&self, 0, sizeof(self));

  self.data = malloc(MUZZY_BUFFER_INITIAL_LEN);
  memset(self.data, 0, MUZZY_BUFFER_INITIAL_LEN);
  self.max_len = MUZZY_BUFFER_INITIAL_LEN;

  return self;
}

void muzzy_buffer_resize(struct muzzy_buffer *self, size_t by) {
  size_t new_max_len = self->max_len + by;
  uint8_t *new_data = realloc(self->data, new_max_len);

  if (new_data) {
    muzzy_dbg(
        "Resized buffer %p:%ld by %ld bytes (to %ld). Old address: %p. New "
        "address: %p\n",
        (void *)self, self->max_len, by, new_max_len, (void *)self->data,
        (void *)new_data);
    self->max_len = new_max_len;
    self->data = new_data;
  } else {
    muzzy_errno();
  }
}

uint8_t *muzzy_buffer_next(struct muzzy_buffer *self, size_t len) {
  if (self->index + len >= self->max_len) {
    muzzy_buffer_resize(self, len * 2);
    if (muzzy_err()) {
      return NULL;
    }
  }

  return self->data + self->index;
}

void muzzy_buffer_adv(struct muzzy_buffer *self, size_t n) { self->index += n; }

void muzzy_buffer_clear(struct muzzy_buffer *self) { self->index = 0; }

const uint8_t *muzzy_buffer_start(struct muzzy_buffer *self) {
  muzzy_dbg_assert(self->data);
  return self->data;
}

size_t muzzy_buffer_len(struct muzzy_buffer *self) { return self->index; }

void muzzy_buffer_null_term(struct muzzy_buffer *self) {
  uint8_t *next = muzzy_buffer_next(self, 1);
  *next = '\0';
}

uint8_t *muzzy_buffer_move(struct muzzy_buffer *self) { return self->data; }

uint8_t *muzzy_buffer_cat(char separator, const char **strings, size_t len) {
  struct muzzy_buffer self = muzzy_buffer_init();
  for (size_t i = 0; i < len; i++) {
    size_t len = strlen(strings[i]);
    uint8_t *next = muzzy_buffer_next(&self, len);
    memcpy(next, strings[i], len);
    muzzy_buffer_adv(&self, len);

    // do not append \0 and also do not append
    // when the next element is the last
    if (separator && i < len - 2) {
      // space between all the commands
      *muzzy_buffer_next(&self, 1) = separator;
      muzzy_buffer_adv(&self, 1);
    }
  }
  muzzy_buffer_null_term(&self);

  return muzzy_buffer_move(&self);
}

void muzzy_buffer_free(struct muzzy_buffer *self) { free(self->data); }
