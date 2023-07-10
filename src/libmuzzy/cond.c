#include "libmuzzy/cond.h"
#include "libmuzzy/vec.h"
#include <string.h>

struct muzzy_cond muzzy_cond_init(void) {
  struct muzzy_cond self;
  memset(&self, 0, sizeof(self));

  self.op = MUZZY_COND_FALSE;

  return self;
}

struct muzzy_cond muzzy_cond_init_ec(enum muzzy_cond_op op, int exit_code) {
  struct muzzy_cond self = muzzy_cond_init();
  self.op = op;
  self.exit_code = exit_code;
  return self;
}

bool muzzy_cond_check(struct muzzy_cond *self, int exit_code, const char *out) {
  switch (self->op) {
  case MUZZY_COND_FALSE:
    return false;
  case MUZZY_COND_TRUE:
    return true;
  case MUZZY_COND_EC_EQ:
    return self->exit_code == exit_code;
  case MUZZY_COND_EC_GT:
    return self->exit_code > exit_code;
  case MUZZY_COND_EC_GTEQ:
    return self->exit_code >= exit_code;
  case MUZZY_COND_EC_LT:
    return self->exit_code < exit_code;
  case MUZZY_COND_EC_LTEQ:
    return self->exit_code <= exit_code;
  case MUZZY_COND_EC_NEQ:
    return self->exit_code != exit_code;
  }

  return false;
}

bool muzzy_conds_check_or(struct muzzy_vec *conds, int exit_code,
                          const char *out) {
  for (size_t i = 0; i < conds->len; i++) {
    struct muzzy_cond *cond = muzzy_vec_get(conds, i);
    if (muzzy_cond_check(cond, exit_code, out)) {
      return true;
    }
  }
  return false;
}

void muzzy_cond_vec_free(struct muzzy_vec *conds) { muzzy_vec_free(conds); }
