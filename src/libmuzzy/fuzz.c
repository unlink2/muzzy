#include "libmuzzy/fuzz.h"
#include "libmuzzy/buffer.h"
#include "libmuzzy/config.h"
#include "libmuzzy/error.h"
#include "libmuzzy/macros.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_words muzzy_words_init(void) {
  struct muzzy_words self;
  memset(&self, 0, sizeof(self));

  self.list = muzzy_vec_init(sizeof(const char *));
  self.replace = MUZZY_DEFAULT_REPLACE_WORD;

  return self;
}

struct muzzy_words muzzy_words_from_file(const char *path, const char *rep) {
  struct muzzy_words self = muzzy_words_init();

  self.replace = rep;

  FILE *f = NULL;
  if (strcmp(MUZZY_STDSTREAM_PATH, path) == 0) {
    f = stdin;
  } else {
    f = fopen(path, "re");
  }

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

  if (f != stdin) {
    fclose(f);
  }

  return self;
}

const char *muzzy_words_next(const char **words, int64_t i) { return words[i]; }

int64_t muzzy_rand_always0(void *data) { return 0; }

char *muzzy_word_rep(const char *input, const char *replace, const char *word,
                     ssize_t n, struct muzzy_buffer *buf) {
  return muzzy_word_rep_rand(input, replace, (const char *[]){word}, 1, n,
                             muzzy_rand_always0, NULL, buf);
}

char *muzzy_word_rep_rand(const char *input, const char *replace,
                          const char **words, size_t words_len, ssize_t n,
                          muzzy_rand rand, struct muzzy_rand_cfg *rand_cfg,
                          struct muzzy_buffer *buf) {
  struct muzzy_buffer buf_local;
  if (!buf) {
    buf_local = muzzy_buffer_init();
    buf = &buf_local;
  } else {
    muzzy_buffer_clear(buf);
  }

  const size_t replace_len = strlen(replace);
  const char *start = input;
  const char *end = input + strlen(input);
  const char *next = NULL;

  while ((next = strstr(start, replace)) && (n == -1 || n--)) {
    // copy head into buffer
    size_t start_len = next - start;
    char *b = (void *)muzzy_buffer_next(buf, start_len);
    memcpy(b, start, start_len);
    muzzy_buffer_adv(buf, start_len);

    // select word
    int64_t rng = rand(&rand_cfg);
    const char *word = words[rng % (int64_t)words_len];

    // copy words into buffer
    size_t word_len = strlen(word);
    char *w = (void *)muzzy_buffer_next(buf, word_len);
    memcpy(w, word, word_len); // NOLINT
    muzzy_buffer_adv(buf, word_len);

    // skip remainder
    start = next + replace_len;
  }

  // copy tail into buffer
  size_t tail_len = end - start;
  char *tail = (void *)muzzy_buffer_next(buf, tail_len);
  memcpy(tail, start, tail_len);
  muzzy_buffer_adv(buf, tail_len);

  muzzy_buffer_null_term(buf);

  return (void *)muzzy_buffer_move(buf);
}

void muzzy_words_free(struct muzzy_words *self) {
  for (size_t i = 0; i < self->list.len; i++) {
    char **s = muzzy_vec_get(&self->list, i);
    free(*s);
  }
  muzzy_vec_free(&self->list);
}
