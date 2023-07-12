#include "libmuzzy/error.h"
#include "libmuzzy/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include "libmuzzy/test/tok.h"
#include "libmuzzy/tok.h"

void test_tok(void **state) {
  const char *input =
      "  tok  test 123 'test token \\' with escaped chars' after";
  char buf[64];

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("tok", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("test", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("123", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("test token ' with escaped chars", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("after", buf);

  input = muzzy_tok_str(buf, input, 64);
  assert_false(muzzy_err());
  assert_string_equal("", buf);

  // unterminated input
  const char *unterm = "'Unterminated";
  input = muzzy_tok_str(buf, unterm, 64);
  assert_int_equal(MUZZY_ERR_UNTERMINATED_TOKEN, muzzy_err());
}
