#include "libmuzzy/fuzz.h"
#include "libmuzzy/macros.h"
#include "libmuzzy/vec.h"
#include <string.h>

struct muzzy_words muzzy_words_init(void) {
  struct muzzy_words self;
  memset(&self, 0, sizeof(self));

  self.list = muzzy_vec_init(sizeof(const char *));
  self.replace = MUZZY_DEFAULT_REPLACE_WORD;
  self.separator = MUZZY_DEFAULT_SEPARATOR;

  return self;
}

struct muzzy_words muzzy_words_from_file(const char *path) {}

void muzzy_words_free(struct muzzy_words *self) { muzzy_vec_free(&self->list); }
