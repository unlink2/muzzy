#include "libmuzzy/test/cond.h"
#include "libmuzzy/test/test.h"
#include "libmuzzy/cond.h"
#include "libmuzzy/vec.h"

void test_cond(void **state) {
  {
    struct muzzy_vec v = muzzy_vec_init(sizeof(struct muzzy_cond));

    struct muzzy_cond c1 =
        muzzy_cond_init_ec(MUZZY_COND_EC_GT, MUZZY_COND_OR, false, 1);
    assert_true(muzzy_cond_check(&c1, 2, ""));
    assert_false(muzzy_cond_check(&c1, 1, ""));

    struct muzzy_cond c2 =
        muzzy_cond_init_ec(MUZZY_COND_EC_LT, MUZZY_COND_OR, false, 3);
    assert_true(muzzy_cond_check(&c2, 2, ""));
    assert_false(muzzy_cond_check(&c2, 3, ""));

    muzzy_vec_push(&v, &c1);
    muzzy_vec_push(&v, &c2);

    assert_true(muzzy_conds_check(&v, 2, "", NULL));
    assert_true(muzzy_conds_check(&v, 3, "", NULL));
    assert_true(muzzy_conds_check(&v, 1, "", NULL));

    muzzy_cond_vec_free(&v);
  }
  {
    struct muzzy_vec v = muzzy_vec_init(sizeof(struct muzzy_cond));

    struct muzzy_cond c1 =
        muzzy_cond_init_ec(MUZZY_COND_EC_GT, MUZZY_COND_AND, false, 1);
    assert_true(muzzy_cond_check(&c1, 2, ""));
    assert_false(muzzy_cond_check(&c1, 1, ""));

    struct muzzy_cond c2 =
        muzzy_cond_init_ec(MUZZY_COND_EC_LT, MUZZY_COND_OR, false, 3);
    assert_true(muzzy_cond_check(&c2, 2, ""));
    assert_false(muzzy_cond_check(&c2, 3, ""));

    muzzy_vec_push(&v, &c1);
    muzzy_vec_push(&v, &c2);

    assert_true(muzzy_conds_check(&v, 2, "", NULL));
    assert_false(muzzy_conds_check(&v, 3, "", NULL));
    assert_false(muzzy_conds_check(&v, 1, "", NULL));

    muzzy_cond_vec_free(&v);
  }
}
