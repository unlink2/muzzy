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

  assert_non_null(res);
  assert_false(muzzy_err());
  assert_string_equal("Test Test1 123", res[0]);
  assert_string_equal("Arg2 Test1 123", res[1]);
  assert_null(res[2]);

  free(res);
  muzzy_attempt_free(&a);
  muzzy_buf_vec_free(&dst);
  muzzy_buf_vec_free(&args);
  muzzy_words_free(&words);
}

void test_attempt_fuzz_args(void **state) {
  struct muzzy_vec buf0 = muzzy_vec_init(sizeof(struct muzzy_buffer));
  struct muzzy_vec buf1 = muzzy_vec_init(sizeof(struct muzzy_buffer));

  struct muzzy_vec words = muzzy_vec_init(sizeof(struct muzzy_words));

  // word list 1
  struct muzzy_words words1 = muzzy_words_init();
  words1.replace = "R1";

  char *s1 = strdup("Test0");
  muzzy_vec_push(&words1.list, &s1);
  char *s2 = strdup("Test1");
  muzzy_vec_push(&words1.list, &s2);
  char *s3 = strdup("Test2");
  muzzy_vec_push(&words1.list, &s3);

  // word list 2
  struct muzzy_words words2 = muzzy_words_init();
  words2.replace = "R2";

  char *s4 = strdup("Test3");
  muzzy_vec_push(&words2.list, &s4);
  char *s5 = strdup("Test4");
  muzzy_vec_push(&words2.list, &s5);
  char *s6 = strdup("Test5");
  muzzy_vec_push(&words2.list, &s6);

  muzzy_vec_push(&words, &words1);
  muzzy_vec_push(&words, &words2);

  const char *args[] = {"Test R1 123 R2", "Arg2 R1 123 R2", NULL};

  const char **res = muzzy_attempt_fuzz_args(NULL, args, &words, &buf0, &buf1,
                                             rand_test, NULL);

  assert_non_null(res);
  assert_false(muzzy_err());
  assert_string_equal("Test Test1 123 Test4", res[0]);
  assert_string_equal("Arg2 Test1 123 Test4", res[1]);
  assert_null(res[2]);

  free(res);
  muzzy_buf_vec_free(&buf0);
  muzzy_buf_vec_free(&buf1);
  muzzy_words_vec_free(&words);
}
