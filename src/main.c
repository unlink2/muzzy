#include <stdio.h>
#include <stdlib.h>
#include "libmuzzy/attempt.h"
#include "libmuzzy/config.h"
#include "libmuzzy/error.h"
#include "libmuzzy/log.h"
#include "arg.h"

int main(int argc, char **argv) {
  struct muzzy_config cfg = muzzy_args_to_config(argc, argv);
  if (!muzzy_err()) {
    struct muzzy_attempt attempt = muzzy_attempt_from_cfg(&cfg);
    muzzy_attempt_run(&attempt);
    muzzy_attempt_free(&attempt);
  }
  muzzy_err_print(stderr);

  return muzzy_err();
}
