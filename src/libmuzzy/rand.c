#include "libmuzzy/rand.h"
#include "libmuzzy/error.h"
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

  self.src_path = path;
  self.fp = fopen(path, "re");
  if (!self.fp) {
    muzzy_errno();
  }

  return self;
}

int64_t muzzy_rand(void *data) { return rand(); } // NOLINT

int64_t muzzy_frand(void *data) {
  struct muzzy_rand_cfg *cfg = data;
  int64_t res = 0;
  if (fread(&res, 1, sizeof(int64_t), cfg->fp) == -1) {
    muzzy_errno();
  }

  return res;
}

void muzzy_frand_cfg_free(struct muzzy_rand_cfg *self) { fclose(self->fp); }
