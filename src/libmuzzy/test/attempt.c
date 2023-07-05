#include "libmuzzy/buffer.h"
#include "libmuzzy/fuzz.h"
#include "libmuzzy/test/test.h"
#include "libmuzzy/attempt.h"
#include "libmuzzy/test/attempt.h"
#include "libmuzzy/vec.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int64_t rand_test(void *cfg) { return 1; }

void test_attempt_words(void **state) {
  struct muzzy_attempt a = muzzy_attempt_init();

  struct muzzy_vec dst = muzzy_vec_init(sizeof(struct muzzy_buffer));
  struct muzzy_vec args = muzzy_vec_init(sizeof(struct muzzy_buffer));

  struct muzzy_words words = muzzy_words_init();
  words.replace = "R1";

  // word list
  char *s1 = strdup("Test0");
  muzzy_vec_push(&words.list, &s1);
  char *s2 = strdup("Test1");
  muzzy_vec_push(&words.list, &s2);
  char *s3 = strdup("Test2");
  muzzy_vec_push(&words.list, &s3);

  muzzy_attempt_args_to_buffer(
      &args, (const char *[]){"Test R1 123", "Arg2 R1 123", NULL});

  muzzy_attempt_words(&dst, &args, &words, rand_test, NULL);
  const char **res = muzzy_attempt_to_exec_args(NULL, &dst);

  assert_string_equal("Test Test1 123", res[0]);
  assert_string_equal("Arg2 Test1 123", res[1]);

  free(res);
  muzzy_attempt_free(&a);

  for (size_t i = 0; i < dst.len; i++) {
    muzzy_buffer_free(muzzy_vec_get(&dst, i));
  }
  muzzy_vec_free(&dst);

  for (size_t i = 0; i < args.len; i++) {
    muzzy_buffer_free(muzzy_vec_get(&args, i));
  }
  muzzy_vec_free(&args);

  muzzy_words_free(&words);
}
