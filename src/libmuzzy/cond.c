#include "libmuzzy/cond.h"
#include "libmuzzy/vec.h"
#include <stdio.h>
#include <string.h>

struct muzzy_cond muzzy_cond_init(void) {
  struct muzzy_cond self;
  memset(&self, 0, sizeof(self));

  self.op = MUZZY_COND_FALSE;

  return self;
}

struct muzzy_cond muzzy_cond_init_ec(enum muzzy_cond_op op,
                                     enum muzzy_cond_con connector, bool not,
                                     int exit_code) {
  struct muzzy_cond self = muzzy_cond_init();
  self.op = op;
  self.exit_code = exit_code;
  self.connector = connector;
  self.not = not ;
  return self;
}

bool muzzy_cond_check(struct muzzy_cond *self, int exit_code, const char *out) {
  switch (self->op) {
  case MUZZY_COND_FALSE:
    return false;
  case MUZZY_COND_TRUE:
    return true;
  case MUZZY_COND_EC_EQ:
    return exit_code == self->exit_code;
  case MUZZY_COND_EC_GT:
    return exit_code > self->exit_code;
  case MUZZY_COND_EC_GTEQ:
    return exit_code >= self->exit_code;
  case MUZZY_COND_EC_LT:
    return exit_code < self->exit_code;
  case MUZZY_COND_EC_LTEQ:
    return exit_code <= self->exit_code;
  }

  return false;
}

bool muzzy_conds_check(struct muzzy_vec *conds, int exit_code, const char *out,
                       size_t *n) {
  size_t loc_n = 0;
  if (n == NULL) {
    n = &loc_n;
  }

  for (; *n < conds->len; (*n)++) {
    struct muzzy_cond *cond = muzzy_vec_get(conds, *n);
    if (muzzy_cond_check(cond, exit_code, out)) {
      if (cond->connector == MUZZY_COND_OR) {
        return true;
      }
      (*n)++;
      if (muzzy_conds_check(conds, exit_code, out, n)) {
        return true;
      }
    } else if (cond->connector == MUZZY_COND_AND) {
      return false;
    }
  }
  return false;
}

void muzzy_cond_vec_free(struct muzzy_vec *conds) { muzzy_vec_free(conds); }
