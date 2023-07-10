#ifndef COND_H_
#define COND_H_

#include "libmuzzy/macros.h"
#include "libmuzzy/vec.h"

enum muzzy_cond_op {
  MUZZY_COND_TRUE,
  MUZZY_COND_FALSE,
  MUZZY_COND_EC_EQ,
  MUZZY_COND_EC_NEQ,
  MUZZY_COND_EC_GT,
  MUZZY_COND_EC_GTEQ,
  MUZZY_COND_EC_LT,
  MUZZY_COND_EC_LTEQ,
};

struct muzzy_cond {
  enum muzzy_cond_op op;
  union {
    int exit_code;
  };
  // TODO: implement and, and or tree of conditions
  // e.g. by parsing --ex-eq '0 --ex-lt 100' into a sub-expression where
  // the first token (0) is simplt the value and all other values after
  // are passed into the cli parser again and used as subsequent values
  // of another vec of or-expressions that is held in the condition
};

struct muzzy_cond muzzy_cond_init(void);

struct muzzy_cond muzzy_cond_init_ec(enum muzzy_cond_op op, int exit_code);

bool muzzy_cond_check(struct muzzy_cond *self, int exit_code, const char *out);

// check all conditions in this vec
bool muzzy_conds_check_or(struct muzzy_vec *conds, int exit_code,
                          const char *out);

void muzzy_cond_vec_free(struct muzzy_vec *conds);

#endif
