#include "libmuzzy/fuzz.h"
#include "libmuzzy/error.h"
#include "libmuzzy/macros.h"
#include "libmuzzy/vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_words muzzy_words_init(void) {
  struct muzzy_words self;
  memset(&self, 0, sizeof(self));

  self.list = muzzy_vec_init(sizeof(const char *));
  self.replace = MUZZY_DEFAULT_REPLACE_WORD;
  self.separator = MUZZY_DEFAULT_SEPARATOR;

  return self;
}

struct muzzy_words muzzy_words_from_file(const char *path, const char *rep,
                                         const char *sep) {
  struct muzzy_words self = muzzy_words_init();

  self.separator = sep;
  self.replace = rep;

  FILE *f = fopen(path, "re");

  if (!f) {
    muzzy_errno();
    return self;
  }

  ssize_t read = 0;
  size_t len = 0;
  char *line_buf = NULL;
  while ((read = getline(&line_buf, &len, f)) != -1) {
    char *next = strdup(line_buf);
    muzzy_vec_push(&self.list, &next);
  }

  free(line_buf);

  fclose(f);

  return self;
}

void muzzy_words_free(struct muzzy_words *self) { muzzy_vec_free(&self->list); }
