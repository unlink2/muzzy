#include "libmuzzy/test/test.h"
#include "libmuzzy/test/vec.h"
#include "libmuzzy/test/buffer.h"

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_vec),
      cmocka_unit_test(test_buffer),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
