#include "libmuzzy/test/buffer.h"
#include "libmuzzy/error.h"
#include "libmuzzy/test/test.h"
#include "libmuzzy/buffer.h"
#include <cmocka.h>
#include <stdlib.h>

void test_buffer(void **state) {
  struct muzzy_buffer b1 = muzzy_buffer_init();

  assert_non_null(muzzy_buffer_next(&b1, MUZZY_BUFFER_INITIAL_LEN - 1));
  assert_false(muzzy_err());
  muzzy_buffer_adv(&b1, MUZZY_BUFFER_INITIAL_LEN - 1);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN, b1.max_len);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN - 1, b1.index);

  assert_non_null(muzzy_buffer_next(&b1, 127));
  assert_false(muzzy_err());
  muzzy_buffer_adv(&b1, 127);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN + 127 * 2, b1.max_len);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN - 1 + 127, b1.index);

  assert_non_null(muzzy_buffer_next(&b1, 127));
  assert_false(muzzy_err());
  muzzy_buffer_adv(&b1, 127);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN + 127 * 2, b1.max_len);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN - 1 + 127 * 2, b1.index);

  assert_non_null(muzzy_buffer_next(&b1, 127));
  assert_false(muzzy_err());
  muzzy_buffer_adv(&b1, 127);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN + 127 * 4, b1.max_len);
  assert_int_equal(MUZZY_BUFFER_INITIAL_LEN - 1 + 127 * 3, b1.index);

  muzzy_buffer_free(&b1);

  // test cat
  uint8_t *cat_res = muzzy_buffer_cat(' ', (const char *[2]){"Test", "cat"}, 2);
  assert_string_equal("Test cat ", (char *)cat_res);
  free(cat_res);
}
