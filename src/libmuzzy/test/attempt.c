#include "libmuzzy/buffer.h"
#include "libmuzzy/fuzz.h"
#include "libmuzzy/test/test.h"
#include "libmuzzy/attempt.h"
#include "libmuzzy/test/attempt.h"
#include "libmuzzy/vec.h"
#include <stdint.h>

int64_t rand_test(void *cfg) { return 1; }

void test_attempt_words(void **state) {
  struct muzzy_attempt a = muzzy_attempt_init();

  struct muzzy_vec dst = muzzy_vec_init(sizeof(struct muzzy_buffer));
  struct muzzy_vec args = muzzy_vec_init(sizeof(struct muzzy_buffer));

  struct muzzy_words words = muzzy_words_init();
  words.replace = "R1";
  muzzy_vec_push(&words.list, &"Test0");
  muzzy_vec_push(&words.list, &"Test1");
  muzzy_vec_push(&words.list, &"Test2");

  muzzy_attempt_words(&dst, &args, &words, rand_test, NULL);

  muzzy_attempt_free(&a);

  muzzy_vec_free(&dst);
  muzzy_vec_free(&args);
  muzzy_words_free(&words);
}
