#include "libmuzzy/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include "libmuzzy/test/fuzz.h"
#include "libmuzzy/fuzz.h"

void test_fuzz(void **state) {
  char *n0 =
      muzzy_words_rep("This is a REPME test REPME for the replace function",
                      "REPME", (const char *[2]){"W1", "W2"}, 2, -1);

  assert_string_equal("This is a W1W2 test W1W2 for the replace function", n0);
  free(n0);

  char *n1 =
      muzzy_words_rep("This is a REPME test REPME for the replace function",
                      "REPME", (const char *[2]){"W1", "W2"}, 2, 1);

  assert_string_equal("This is a W1W2 test REPME for the replace function", n1);
  free(n1);
}
