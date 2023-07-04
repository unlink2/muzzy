#ifndef ATTEMPT_H_
#define ATTEMPT_H_

#include "libmuzzy/config.h"
#include "libmuzzy/error.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"
#include "libmuzzy/buffer.h"
#include <stdio.h>

#define MUZZY_DEFAULT_EXECUTABLE "/bin/sh"
#define MUZZY_DEFAULT_EXECUTABLE_ARG0 "-c"
#define MUZZY_DEFAULT_EXECUTABLE_ENV "/usr/bin/env"

// an attempt is the execution enviornment for a fuzzer
// execute the fuzzer based in all word lists,
// run the command provided and apply all comparisons
// finally it will output based on the config
// several attempts can run indipendently in threads
struct muzzy_attempt {
  // vec of word lists
  struct muzzy_vec word_lists;
  struct muzzy_rand_cfg rand_cfg;
  const char *executable;

  // array of args NULL terminated
  const char **args;
  // dynamically created buffer for chars that
  // were modified. NULL terminated
  const char **modified_args_buf;

  struct muzzy_buffer out;

  // dump out to this file
  FILE *out_to;
  muzzy_rand rand;

  // conditions (muzzy_cond)
  struct muzzy_vec cond_list;

  int32_t n_runs;
  bool recursive;
  bool dry;
};

struct muzzy_attempt muzzy_attempt_init(void);

struct muzzy_attempt muzzy_attempt_from_cfg(struct muzzy_config *cfg);

// runs a single execution
// run a new attempt either sync or in a thread with the attempt configuration.
// Note: Passing an attempt configuration to more than one run call is
// not safe!
int muzzy_attempt_run(struct muzzy_attempt *self);

// runs until an exit condition is met
int muzzy_attempt_run_all(struct muzzy_attempt *self);

void muzzy_attempt_free(struct muzzy_attempt *self);

#endif
