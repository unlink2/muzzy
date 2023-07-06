#include <stdio.h>
#include <stdlib.h>
#include "libmuzzy/attempt.h"
#include "libmuzzy/config.h"
#include "libmuzzy/error.h"
#include "libmuzzy/log.h"
#include "arg.h"

int main(int argc, char **argv) {
  struct muzzy_config cfg = muzzy_args_to_config(argc, argv);
  struct muzzy_attempt attempt = muzzy_attempt_from_cfg(&cfg);
  muzzy_attempt_run(&attempt);

  if (muzzy_err()) {
    fprintf(stderr, "%s\n", muzzy_err_to_str(muzzy_err()));
  }

  return muzzy_err();
}
