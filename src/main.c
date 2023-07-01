#include <stdio.h>
#include <stdlib.h>
#include "libmuzzy/config.h"
#include "libmuzzy/log.h"
#include "arg.h"

int main(int argc, char **argv) {
  struct muzzy_config cfg = muzzy_args_to_config(argc, argv);
  return 0;
}
