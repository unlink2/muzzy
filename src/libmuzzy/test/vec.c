#include "libmuzzy/test/test.h"
#include <cmocka.h>
#include "libmuzzy/test/vec.h"
#include "libmuzzy/vec.h"
#include <stdint.h>

void test_vec(void **state) {
  struct muzzy_vec v = muzzy_vec_init(sizeof(intptr_t));

  assert_null(muzzy_vec_pop(&v));
  assert_int_equal(0, v.len);

  for (int i = 0; i < MUZZY_VEC_DEFAULT_MAX_LEN * 4; i++) {
    int d = 0 - i;
    muzzy_vec_push(&v, &d);
    assert_int_equal(i + 1, v.len);
  }

  for (int i = 0; i < MUZZY_VEC_DEFAULT_MAX_LEN * 4; i++) {
    int *r = muzzy_vec_get(&v, i);
    assert_int_equal(0 - i, *r);
    int *p = muzzy_vec_pop(&v);
    assert_int_equal(0 - i, *p);
  }

  muzzy_vec_free(&v);
}
