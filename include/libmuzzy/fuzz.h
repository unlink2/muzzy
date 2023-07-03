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

// apply a random word  to the input string
// if the replace word appears in the input string this will malloc a new
// string. If the operation was successful return the allocated string and
// the location where the replace sequence was replaced in *at as well as the
// length of it in *rep_len.
//
// if it does not this will simply return the input string and set *at
// and *end to NULL to signal that no malloc was performed.
const char *muzzy_words_apply(const struct muzzy_words *self, const char *input,
                              ssize_t *at, size_t *rep_len);

// insert the separator and repalce string at the specified location
// 'at'. This will allocate a new string and insert the data at the specified
// location + len.
// Returns the newly allocated string on success
const char *muzzy_words_ins_rep(const struct muzzy_words *self,
                                const char *input, size_t at);

void muzzy_words_free(struct muzzy_words *self);

#endif
