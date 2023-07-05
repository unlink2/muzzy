#include "libmuzzy/attempt.h"
#include "libmuzzy/buffer.h"
#include "libmuzzy/error.h"
#include "libmuzzy/fuzz.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_attempt muzzy_attempt_init(void) {
  struct muzzy_attempt self;
  memset(&self, 0, sizeof(self));

  self.out = muzzy_buffer_init();
  self.buf0 = muzzy_vec_init(sizeof(struct muzzy_buffer));
  self.buf1 = muzzy_vec_init(sizeof(struct muzzy_buffer));

  return self;
}

struct muzzy_attempt muzzy_attempt_from_cfg(struct muzzy_config *cfg) {
  struct muzzy_attempt self = muzzy_attempt_init();

  size_t args_len = 1; // start at 1 to insert NULL in the end
  const char **arg = self.args;
  while (arg[0]) {
    // push the right amount of buffers to the target
    struct muzzy_buffer b0 = muzzy_buffer_init();
    struct muzzy_buffer b1 = muzzy_buffer_init();

    muzzy_vec_push(&self.buf0, &b0);
    muzzy_vec_push(&self.buf1, &b1);

    arg++;
  }

  // result buffer used for storage of the actual pointers
  self.args_fuzzed = malloc(sizeof(char *) * args_len + 1);
  memset(self.args_fuzzed, 0, sizeof(char *) * args_len + 1);

  return self;
}

int muzzy_attempt_dry(struct muzzy_attempt *self) {}

struct muzzy_vec *muzzy_attempt_words(struct muzzy_vec *dst,
                                      struct muzzy_vec *args,
                                      struct muzzy_words *wl, muzzy_rand rand,
                                      struct muzzy_rand_cfg *rand_cfg) {
  // fuzz the input
  for (size_t i = 0; i < args->len; i++) {
    int64_t rng = rand(&rand_cfg);

    const char *word =
        muzzy_words_next(wl->list.data, rng % (int64_t)wl->list.len);

    const char **arg = muzzy_vec_get(args, i);
    struct muzzy_buffer *buf = muzzy_vec_get(dst, i);

    if (!buf) {
      struct muzzy_buffer b = muzzy_buffer_init();
      muzzy_vec_push(dst, &b);
      buf = muzzy_vec_get(dst, i);
    }

    muzzy_word_rep(*arg, wl->replace, word, -1, buf);
  }
  return dst;
}

void muzzy_attempt_args_to_buffer(struct muzzy_vec *dst, const char **src) {
  size_t i = 0;
  while (*src) {
    struct muzzy_buffer *b = muzzy_vec_get(dst, i);

    if (!b) {
      struct muzzy_buffer nb = muzzy_buffer_init();
      muzzy_vec_push(dst, &nb);
      b = muzzy_vec_get(dst, i);
    }

    muzzy_buffer_clear(b);

    size_t len = strlen(*src);
    memcpy(muzzy_buffer_next(b, len), *src, len);
    muzzy_buffer_adv(b, len);
    muzzy_buffer_null_term(b);

    src++;
    i++;
  }
}

const char **muzzy_attempt_to_exec_args(const char **dst,
                                        struct muzzy_vec *src) {
  if (dst == NULL) {
    dst = malloc(sizeof(char *) * (src->len + 1));
  }

  for (size_t i = 0; i < src->len; i++) {
    struct muzzy_buffer *b = muzzy_vec_get(src, i);
    dst[i] = (const char *)b->data;
  }

  dst[src->len] = NULL;
  return dst;
}

int muzzy_attempt_exec(struct muzzy_attempt *self) {
  struct muzzy_vec *args_buf = &self->buf0;
  struct muzzy_vec *dst_buf = &self->buf1;

  for (size_t i = 0; i < self->word_lists.len; i++) {
    struct muzzy_words *word_list = muzzy_vec_get(&self->word_lists, i);
    muzzy_attempt_words(dst_buf, args_buf, word_list, self->rand,
                        &self->rand_cfg);

    // swap for next iteration
    struct muzzy_vec *tmp = args_buf;
    args_buf = dst_buf;
    dst_buf = tmp;
  }

  if (self->dry) {
    return muzzy_attempt_dry(self);
  }

  // TODO: exec

  // swap buffers for next iteration

  return 0;
}

void muzzy_attempt_out(struct muzzy_attempt *self) {
  if (fwrite(muzzy_buffer_start(&self->out), 1, muzzy_buffer_len(&self->out),
             self->out_to) == -1) {
    muzzy_errno();
  }
}

int muzzy_attempt_run(struct muzzy_attempt *self) {
  muzzy_buffer_clear(&self->out);
  // act
  int act_exit_code = muzzy_attempt_exec(self);

  // condition

  // output

  return act_exit_code;
}

void muzzy_attempt_free(struct muzzy_attempt *self) {
  for (size_t i = 0; i < self->buf0.len; i++) {
    muzzy_buffer_free(muzzy_vec_get(&self->buf0, i));
  }
  muzzy_vec_free(&self->buf0);

  for (size_t i = 0; i < self->buf1.len; i++) {
    muzzy_buffer_free(muzzy_vec_get(&self->buf1, i));
  }
  muzzy_vec_free(&self->buf1);

  muzzy_rand_cfg_free(&self->rand_cfg);
  muzzy_buffer_free(&self->out);

  free(self->args_fuzzed);
}
