#ifndef COND_H_
#define COND_H_

#include "libmuzzy/macros.h"
#include "libmuzzy/vec.h"

enum muzzy_cond_op {
  MUZZY_COND_TRUE,
  MUZZY_COND_FALSE,
  MUZZY_COND_EC_EQ,
  MUZZY_COND_EC_GT,
  MUZZY_COND_EC_GTEQ,
  MUZZY_COND_EC_LT,
  MUZZY_COND_EC_LTEQ,

  MUZZY_COND_CONTAINS,
};

enum muzzy_cond_con {
  MUZZY_COND_AND,
  MUZZY_COND_OR,
};

struct muzzy_cond {
  enum muzzy_cond_op op;
  enum muzzy_cond_con connector;
  bool not ;
  union {
    int exit_code;
    const char *contains;
  };
};

struct muzzy_cond muzzy_cond_init(void);

// strtok all arguments
// first arg can either be
// 'not' or an operator
// valid numeric operators:
//  >, <, >=, <=, ==
// valid string operators:
//   !!!Currently not implemented!!!
// the second or third value is the expression comparison value
// this can either be a number or string depending on the operator types.
// The last argument may be either empty, 'and' or 'or'. This will form a
// connection for the next operation
struct muzzy_cond muzzy_cond_from(const char *expr);

struct muzzy_cond muzzy_cond_init_ec(enum muzzy_cond_op op,
                                     enum muzzy_cond_con connector, bool not,
                                     int exit_code);

struct muzzy_cond muzzy_cond_init_out_cmp(enum muzzy_cond_op op,
                                          enum muzzy_cond_con connector,
                                          bool not, const char *tok);

bool muzzy_cond_check(struct muzzy_cond *self, int exit_code, const char *out);

// check all conditions in this vec
bool muzzy_conds_check(struct muzzy_vec *conds, int exit_code, const char *out,
                       size_t n);

void muzzy_conds_dbg_print(struct muzzy_vec *conds, int exit_code,
                           const char *out, size_t n);

void muzzy_cond_free(struct muzzy_cond *self);
void muzzy_cond_vec_free(struct muzzy_vec *conds);

#endif
