#include "libmuzzy/test/test.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/test/rand.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void test_iter_rand(void **state) {
  const int max = 4;
  struct muzzy_rand_cfg cfg = muzzy_rand_cfg_iter(max);

  const size_t len = 16;
  const int64_t tbl0[] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
  const int64_t tbl1[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
  const int64_t tbl2[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

  for (size_t i = 0; i < len; i++) {
    assert_int_equal(tbl0[i], muzzy_iter_rand(0, &cfg) % max);
    assert_int_equal(tbl1[i], muzzy_iter_rand(1, &cfg) % max);
    assert_int_equal(tbl2[i], muzzy_iter_rand(2, &cfg) % max);
  }
}
