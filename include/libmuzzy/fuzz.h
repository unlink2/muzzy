#ifndef FUZZ_H_
#define FUZZ_H_

#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"
#include "libmuzzy/buffer.h"
#include <stdint.h>
#include <stdio.h>

#define MUZZY_DEFAULT_REPLACE_WORD "FUZZ"
#define MUZZY_DEFAULT_SEPARATOR ""
#define MUZZY_PATH_SEPARATOR ':'

struct muzzy_words {
  struct muzzy_vec list;
  const char *replace;

  // this is a buffered value that is used by apply
  // to check if the buffer can hold all possible values
  size_t max_word_len;
};

struct muzzy_words muzzy_words_init(void);

struct muzzy_words muzzy_words_from_file(const char *path, const char *rep);

// obtain a random word from the word list
// it requires an index number that can be provided by any source
// (most commonly an rng function call)
const char *muzzy_words_next(const char **words, int64_t i);

// apply a words to the input string. The words are appended in order
// All occurances of 'replace' are replaced with the apropriate list of words
// If n is set to -1 all words are replaced, otherwise it will stop when no word
// is left to replace
char *muzzy_word_rep(const char *input, const char *replace, const char *word,
                     ssize_t n, struct muzzy_buffer *buf);
char *muzzy_word_rep_rand(const char *input, const char *replace,
                          const char **words, size_t words_len, ssize_t n,
                          muzzy_rand rand, struct muzzy_rand_cfg *rand_cfg,
                          struct muzzy_buffer *buf);

void muzzy_words_free(struct muzzy_words *self);

#endif
