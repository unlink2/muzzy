#include "libmuzzy/config.h"
#include "libmuzzy/vec.h"
#include "libmuzzy/fuzz.h"
#include <string.h>

struct muzzy_config muzzy_config_init(void) {
  struct muzzy_config c;
  memset(&c, 0, sizeof(c));

  c.word_lists = muzzy_vec_init(sizeof(struct muzzy_words));

  return c;
}

void muzzy_config_free(struct muzzy_config *self) {}
