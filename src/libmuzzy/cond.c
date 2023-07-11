#include "libmuzzy/cond.h"
#include "libmuzzy/log.h"
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
  if (!self) {
    return false;
  }

  bool res = false;

  switch (self->op) {
  case MUZZY_COND_FALSE:
    res = false;
    break;
  case MUZZY_COND_TRUE:
    res = true;
    break;
  case MUZZY_COND_EC_EQ:
    res = exit_code == self->exit_code;
    break;
  case MUZZY_COND_EC_GT:
    res = exit_code > self->exit_code;
    break;
  case MUZZY_COND_EC_GTEQ:
    res = exit_code >= self->exit_code;
    break;
  case MUZZY_COND_EC_LT:
    res = exit_code < self->exit_code;
    break;
  case MUZZY_COND_EC_LTEQ:
    res = exit_code <= self->exit_code;
    break;
  }

  if (self->not ) {
    res = !res;
  }

  return res;
}

bool muzzy_conds_check(struct muzzy_vec *conds, int exit_code, const char *out,
                       size_t n) {
  // muzzy_conds_dbg_print(conds, exit_code, out, n);

  while (n < conds->len) {
    bool res = true;
    struct muzzy_cond *cond = NULL;
    do {
      cond = muzzy_vec_get(conds, n);
      if (cond) {
        res &= muzzy_cond_check(cond, exit_code, out);
        n++;
      }
    } while (cond && cond->connector == MUZZY_COND_AND);
    if (res) {
      return res;
    }
  }
  return false;
}

void muzzy_conds_dbg_print(struct muzzy_vec *conds, int exit_code,
                           const char *out, size_t n) {
  while (n < conds->len) {
    struct muzzy_cond *cond = muzzy_vec_get(conds, n);

    if (cond->not ) {
      muzzy_dbg(" not");
    }

    switch (cond->op) {
    case MUZZY_COND_TRUE:
    case MUZZY_COND_FALSE:
      break;
    case MUZZY_COND_EC_EQ:
    case MUZZY_COND_EC_GT:
    case MUZZY_COND_EC_GTEQ:
    case MUZZY_COND_EC_LT:
    case MUZZY_COND_EC_LTEQ:
      muzzy_dbg(" %d", exit_code);
      break;
    }

    switch (cond->op) {
    case MUZZY_COND_TRUE:
      muzzy_dbg(" true");
      break;
    case MUZZY_COND_FALSE:
      muzzy_dbg(" false");
      break;
    case MUZZY_COND_EC_EQ:
      muzzy_dbg(" ==");
      break;
    case MUZZY_COND_EC_GT:
      muzzy_dbg(" >");
      break;
    case MUZZY_COND_EC_GTEQ:
      muzzy_dbg(" >=");
      break;
    case MUZZY_COND_EC_LT:
      muzzy_dbg(" <");
      break;
    case MUZZY_COND_EC_LTEQ:
      muzzy_dbg(" <=");
      break;
    }

    switch (cond->op) {
    case MUZZY_COND_TRUE:
    case MUZZY_COND_FALSE:
      break;
    case MUZZY_COND_EC_EQ:
    case MUZZY_COND_EC_GT:
    case MUZZY_COND_EC_GTEQ:
    case MUZZY_COND_EC_LT:
    case MUZZY_COND_EC_LTEQ:
      muzzy_dbg(" %d", cond->exit_code);
      break;
    }

    switch (cond->connector) {
    case MUZZY_COND_AND:
      muzzy_dbg(" and");
      break;
    case MUZZY_COND_OR:
      muzzy_dbg(" or");
      break;
    }

    n++;
  }
  muzzy_dbg("\n");
}

void muzzy_cond_vec_free(struct muzzy_vec *conds) { muzzy_vec_free(conds); }
