#include "libmuzzy/log.h"

#include <stdint.h>

enum muzzy_loglevel MUZZY_LOG_LEVEL;
FILE *MUZZY_LOG_TARGET;

void muzzy_log_init(enum muzzy_loglevel level) {
  MUZZY_LOG_LEVEL = level;
  MUZZY_LOG_TARGET = stderr;
}
