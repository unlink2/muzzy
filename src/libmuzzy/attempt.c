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

  return self;
}

struct muzzy_attempt muzzy_attempt_from_cfg(struct muzzy_config *cfg) {
  struct muzzy_attempt self = muzzy_attempt_init();

  size_t args_len = 1; // start at 1 to insert NULL in the end
  const char **arg = self.args;
  while (arg[0]) {
    arg++;
    args_len++;
  }

  self.modified_args_buf = malloc(sizeof(char *) * args_len);
  memset(self.modified_args_buf, 0, sizeof(char *) * args_len);

  return self;
}

void muzzy_attempt_args_buf_free(const char **modified_args_buf) {
  const char **arg = modified_args_buf;
  while (arg[0]) {
    free((void *)arg[0]);
    arg++;
  }
}

int muzzy_attempt_dry(struct muzzy_attempt *self) {}

int muzzy_attempt_words(struct muzzy_vec *word_lists, const char **args,
                        const char **modified_args_buf, muzzy_rand rand,
                        struct muzzy_rand_cfg *rand_cfg) {
  // fuzz the input
  muzzy_attempt_args_buf_free(modified_args_buf);
  const char **arg = args;
  const char **buf = modified_args_buf;
  while (arg[0]) {
    int64_t rng = rand(&rand_cfg);
    for (size_t i = 0; i < word_lists->len; i++) {
      struct muzzy_words *wl = muzzy_vec_get(word_lists, i);
      const char *word = muzzy_words_next(wl, rng % (int64_t)wl->list.len);

      buf[0] = muzzy_word_rep(arg[0], wl->replace, word, -1);
    }
    buf++;
    arg++;
  }
  return 0;
}

int muzzy_attempt_exec(struct muzzy_attempt *self) {
  muzzy_attempt_words(&self->word_lists, self->args, self->modified_args_buf,
                      self->rand, &self->rand_cfg);

  if (self->dry) {
    return muzzy_attempt_dry(self);
  }

  // TODO: exec
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
  muzzy_rand_cfg_free(&self->rand_cfg);
  muzzy_attempt_args_buf_free(self->modified_args_buf);
  free(self->modified_args_buf);
}
