#include "libmuzzy/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include "libmuzzy/test/tok.h"
#include "libmuzzy/tok.h"

void test_tok(void **state) {
  const char *input = "  tok  test 123";
  char buf[64];

  input = muzzy_tok_str(buf, input, 64);
  assert_string_equal("tok", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_string_equal("test", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_string_equal("123", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_string_equal("", buf);
}
