#ifndef ATTEMPT_H_
#define ATTEMPT_H_

#include "libmuzzy/error.h"
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
  const char *executable;

  // array of args NULL terminated
  const char **args;

  FILE *out;

  bool dry;
};

#endif
