#include "libmuzzy/attempt.h"
#include "libmuzzy/buffer.h"
#include "libmuzzy/error.h"
#include "libmuzzy/fuzz.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

struct muzzy_attempt muzzy_attempt_init(void) {
  struct muzzy_attempt self;
  memset(&self, 0, sizeof(self));

  self.out = muzzy_buffer_init();

  return self;
}

struct muzzy_attempt muzzy_attempt_from_cfg(struct muzzy_config *cfg) {
  struct muzzy_attempt self = muzzy_attempt_init();

  return self;
}

int muzzy_attempt_dry(struct muzzy_attempt *self) {}

int muzzy_attempt_exec(struct muzzy_attempt *self) {
  // fuzz the input
  const char *arg = self->args[0];
  while (arg) {
    int64_t rng = self->rand(&self->rand_cfg);
    arg++;
  }

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
}
