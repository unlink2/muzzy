#include "libmuzzy/tok.h"
#include "libmuzzy/error.h"
#include <ctype.h>
#include <string.h>

const char *muzzy_tok_str(char *buf, const char *input, size_t n) {
  size_t written = 0;

  while (isspace(*input) && *input != '\0') {
    input++;
  }

  memset(buf, 0, n);

  char end = '\0';
  if (*input == '\'' || *input == '\"') {
    end = *input;
    input++;
  }

  while (written < (n - 1) && (*input != end && *input != '\0')) {
    if (isspace(*input) && end == '\0') {
      break;
    }
    // skip escape character
    if (*input == '\\') {
      input++;
    }
    buf[written] = *input;
    input++;
    written++;
  }

  if (*input != end && end != '\0') {
    muzzy_err_set(MUZZY_ERR_UNTERMINATED_TOKEN);
  } else if (end != '\0') {
    input++;
  }

  return input;
}
