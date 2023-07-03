#include "libmuzzy/fuzz.h"
#include "libmuzzy/buffer.h"
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

char *muzzy_words_rep(const char *input, const char *replace,
                      const char **words, size_t len, ssize_t n) {
  struct muzzy_buffer buf = muzzy_buffer_init();

  const size_t replace_len = strlen(replace);
  const char *start = input;
  const char *end = input + strlen(input);
  const char *next = NULL;

  while ((next = strstr(start, replace)) && (n == -1 || n--)) {
    // copy head into buffer
    size_t start_len = next - start;
    char *b = (void *)muzzy_buffer_next(&buf, start_len);
    memcpy(b, start, start_len);
    muzzy_buffer_adv(&buf, start_len);

    // copy words into buffer
    for (size_t i = 0; i < len; i++) {
      size_t word_len = strlen(words[i]);
      char *w = (void *)muzzy_buffer_next(&buf, word_len);
      memcpy(w, words[i], word_len);
      muzzy_buffer_adv(&buf, word_len);
    }

    // skip remainder
    start = next + replace_len;
  }

  // copy tail into buffer
  size_t tail_len = end - start;
  char *tail = (void *)muzzy_buffer_next(&buf, tail_len);
  memcpy(tail, start, tail_len);
  muzzy_buffer_adv(&buf, tail_len);

  muzzy_buffer_null_term(&buf);

  return (void *)muzzy_buffer_move(&buf);
}

void muzzy_words_free(struct muzzy_words *self) {
  for (size_t i = 0; i < self->list.len; i++) {
    char **s = muzzy_vec_get(&self->list, i);
    free(*s);
  }
  muzzy_vec_free(&self->list);
}
