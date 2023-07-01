#include "libmuzzy/config.h"
#include <string.h>

struct muzzy_config muzzy_config_init(void) {
  struct muzzy_config c;
  memset(&c, 0, sizeof(c));

  return c;
}
