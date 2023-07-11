#include "libmuzzy/cond.h"
#include "libmuzzy/error.h"
#include "libmuzzy/log.h"
#include "libmuzzy/tok.h"
#include "libmuzzy/vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_cond muzzy_cond_init(void) {
  struct muzzy_cond self;
  memset(&self, 0, sizeof(self));

  self.op = MUZZY_COND_FALSE;

  return self;
}

struct muzzy_cond muzzy_cond_from(const char *expr) {
  const size_t len = 64;
  char buf[len];

  muzzy_dbg("Parsing condition '%s'\n", expr);

  struct muzzy_cond self = muzzy_cond_init();
  expr = muzzy_tok_str(buf, expr, len);

  bool not = false;
  enum muzzy_cond_op op = MUZZY_COND_FALSE;
  enum muzzy_cond_con con = MUZZY_COND_OR;
  int int_val = 0;

  // check for not
  if (strncmp(buf, "not", len) == 0) {
    not = true;
    expr = muzzy_tok_str(buf, expr, len);
  }

  // check operaotr
  if (strncmp(buf, ">", len) == 0) {
    op = MUZZY_COND_EC_GT;
  } else if (strncmp(buf, ">=", len) == 0) {
    op = MUZZY_COND_EC_GTEQ;
  } else if (strncmp(buf, "<", len) == 0) {
    op = MUZZY_COND_EC_LT;
  } else if (strncmp(buf, "<=", len) == 0) {
    op = MUZZY_COND_EC_LTEQ;
  } else if (strncmp(buf, "==", len) == 0) {
    op = MUZZY_COND_EC_EQ;
  } else {
    muzzy_err_set(MUZZY_ERR_COND_PARSE);
    muzzy_err_detail(expr, strlen(expr));
  }

  // check value
  expr = muzzy_tok_str(buf, expr, len);
  switch (op) {
  case MUZZY_COND_TRUE:
  case MUZZY_COND_FALSE:
    break;
  case MUZZY_COND_EC_EQ:
  case MUZZY_COND_EC_GT:
  case MUZZY_COND_EC_GTEQ:
  case MUZZY_COND_EC_LT:
  case MUZZY_COND_EC_LTEQ:
    errno = 0;
    int_val = (int)strtol(buf, NULL, 0);
    if (errno) {
      muzzy_errno();
    }
    expr = muzzy_tok_str(buf, expr, len);
    break;
  }

  // check connector
  if (strncmp(buf, "and", len) == 0) {
    con = MUZZY_COND_AND;
  } else if (strncmp(buf, "or", len) == 0) {
    con = MUZZY_COND_OR;
  } else if (buf[0] != '\0') {
    muzzy_err_set(MUZZY_ERR_COND_PARSE);
    muzzy_err_detail(expr, strlen(expr));
  }

  // init
  switch (op) {
  case MUZZY_COND_TRUE:
  case MUZZY_COND_FALSE:
    break;
  case MUZZY_COND_EC_EQ:
  case MUZZY_COND_EC_GT:
  case MUZZY_COND_EC_GTEQ:
  case MUZZY_COND_EC_LT:
  case MUZZY_COND_EC_LTEQ:
    self = muzzy_cond_init_ec(op, con, not, int_val);
    break;
  }

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
