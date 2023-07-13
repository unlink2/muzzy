#include "libmuzzy/rand.h"
#include "libmuzzy/error.h"
#include "libmuzzy/log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_rand_cfg muzzy_rand_cfg_init(void) {
  struct muzzy_rand_cfg self;
  memset(&self, 0, sizeof(self));

  return self;
}

struct muzzy_rand_cfg muzzy_rand_cfg_file(const char *path) {
  struct muzzy_rand_cfg self = muzzy_rand_cfg_init();

  self.kind = MUZZY_FRAND;
  self.src_path = path;
  muzzy_dbg("Rand file: '%s'\n", path);
  self.fp = fopen(path, "re");
  if (!self.fp) {
    muzzy_error("Opening '%s' failed!\n", path);
    muzzy_errno();
  }

  return self;
}

struct muzzy_rand_cfg muzzy_rand_cfg_iter(int max) {
  struct muzzy_rand_cfg self = muzzy_rand_cfg_init();
  self.next_after = max;
  return self;
}

int64_t muzzy_stdrand(int id, void *data) { return rand(); } // NOLINT

int64_t muzzy_frand(int id, void *data) {
  struct muzzy_rand_cfg *cfg = data;
  int64_t res = 0;
  if (fread(&res, 1, sizeof(int64_t), cfg->fp) == -1) {
    muzzy_errno();
  }

  return res;
}

int64_t muzzy_lrand(int id, void *data) {
  struct muzzy_rand_cfg *cfg = data;
  return cfg->linear++;
}

// TODO: test this implementation
int64_t muzzy_iter_rand(int id, void *data) {
  struct muzzy_rand_cfg *cfg = data;
  if (id > MUZZY_RAND_ITER_MAX || cfg->next_after == 0) {
    return 0;
  }

  int64_t next = cfg->iter[id];

  if (id == 0 ||
      (cfg->iter[id - 1] % cfg->next_after == 0 && cfg->iter[id - 1])) {
    cfg->iter[id]++;
  }

  return next;
}

void muzzy_rand_cfg_free(struct muzzy_rand_cfg *self) {
  switch (self->kind) {
  case MUZZY_FRAND:
    muzzy_frand_cfg_free(self);
    break;
  default:
    break;
  }
}

void muzzy_frand_cfg_free(struct muzzy_rand_cfg *self) { fclose(self->fp); }
