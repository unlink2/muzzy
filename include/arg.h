#ifndef ARG_H_
#define ARG_H_

#include "argtable2.h"
#include <stdlib.h>

#include "libmuzzy/config.h"
#include "libmuzzy/log.h"
#include "libmuzzy/macros.h"

struct muzzy_config muzzy_args_to_config(int argc, char **argv);

#endif
