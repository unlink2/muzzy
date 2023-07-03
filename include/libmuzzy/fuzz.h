#ifndef FUZZ_H_
#define FUZZ_H_

#include "libmuzzy/vec.h"

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
// if the replace word appears in the input string
// if it does not this will simply return the input string
// If a sufficient buffer is not provided NULL will be returned
// len will contain the minimum lenght of buf that is expected for the call to
// succeed. If the call is successful the location
// where the word was inserted is returned and len will contain its length.
const char *muzzy_words_apply(const struct muzzy_words *self, const char *input,
                              char *buf, size_t buf_len, size_t *len);

// insert the separator as well as the replace word
// back into the buffer string
// if the buffer is too small NULL is returned and len is set to the expected
// length. If the call succeeded the location of the insertion is returned and
// len is set to the total inserted length in bytes
const char *muzzy_words_ins_rep(const struct muzzy_words *self, const char *buf,
                                size_t buf_len, size_t *len);

void muzzy_words_free(struct muzzy_words *self);

#endif
