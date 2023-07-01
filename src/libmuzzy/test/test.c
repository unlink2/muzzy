#include "libmuzzy/test/test.h"
#include "libmuzzy/test/vec.h"

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_vec),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

