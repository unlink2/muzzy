#include "libmuzzy/test/test.h"
#include "libmuzzy/log.h"
#include "libmuzzy/test/cond.h"
#include "libmuzzy/test/vec.h"
#include "libmuzzy/test/buffer.h"
#include "libmuzzy/test/fuzz.h"
#include "libmuzzy/test/attempt.h"
#include "libmuzzy/test/tok.h"
#include "libmuzzy/test/rand.h"

int main(int arc, char **argv) {
  muzzy_log_init(MUZZY_LOG_LEVEL_DBG);

  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_vec),
      cmocka_unit_test(test_buffer),
      cmocka_unit_test(test_fuzz),
      cmocka_unit_test(test_attempt_words),
      cmocka_unit_test(test_attempt_fuzz_args),
      cmocka_unit_test(test_cond),
      cmocka_unit_test(test_tok),
      cmocka_unit_test(test_iter_rand),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
