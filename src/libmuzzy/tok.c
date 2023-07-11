#include "libmuzzy/tok.h"
#include <ctype.h>
#include <string.h>

const char *muzzy_tok_str(char *buf, const char *input, size_t n) {
  size_t written = 0;

  while (isspace(*input) && *input != '\0') {
    input++;
  }

  memset(buf, 0, n);
  while (written < (n - 1) && *input != '\0') {
    if (isspace(*input)) {
      break;
    }
    buf[written] = *input;
    input++;
    written++;
  }

  return input;
}
