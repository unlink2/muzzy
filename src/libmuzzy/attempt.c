#include "libmuzzy/attempt.h"
#include "libmuzzy/buffer.h"
#include <limits.h>
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

int muzzy_attempt_exec(struct muzzy_attempt *self) {}

int muzzy_attempt_run(struct muzzy_attempt *self) {
  // act
  if (self->dry) {
    // run on string data only
    return muzzy_attempt_dry(self);
  } else {
    return muzzy_attempt_exec(self);
  }

  // condition

  // output
}

void muzzy_attempt_free(struct muzzy_attempt *self) {
  muzzy_rand_cfg_free(&self->rand_cfg);
}
