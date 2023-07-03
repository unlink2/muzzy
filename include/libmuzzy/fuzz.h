#ifndef FUZZ_H_
#define FUZZ_H_

#include "libmuzzy/vec.h"
#include <stdio.h>

#define MUZZY_DEFAULT_REPLACE_WORD "FUZZ"
#define MUZZY_DEFAULT_SEPARATOR ""
#define MUZZY_PATH_SEPARATOR ':'

struct muzzy_words {
  struct muzzy_vec list;
  const char *replace;
  const char *separator;

  // this is a buffered value that is used by apply
  // to check if the buffer can hold all possible values
  size_t max_word_len;
};

struct muzzy_words muzzy_words_init(void);

struct muzzy_words muzzy_words_from_file(const char *path, const char *rep,
                                         const char *sep);

// obtain a random word from the word list
const char *muzzy_words_next(const struct muzzy_words *self);

// apply a words to the input string. The words are appended in order
// All occurances of 'replace' are replaced with the apropriate list of words
const char *muzzy_words_apply(const char *replace, const char **words,
                              size_t len);

void muzzy_words_free(struct muzzy_words *self);

#endif
