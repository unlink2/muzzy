#ifndef LOG_H_
#define LOG_H_

#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

enum muzzy_loglevel {
  MUZZY_LOG_LEVEL_OFF,
  MUZZY_LOG_LEVEL_ERROR,
  MUZZY_LOG_LEVEL_WARN,
  MUZZY_LOG_LEVEL_INFO,
  MUZZY_LOG_LEVEL_DBG,
};

void muzzy_log_init(enum muzzy_loglevel level);

extern enum muzzy_loglevel MUZZY_LOG_LEVEL;
extern FILE *MUZZY_LOG_TARGET;

#define muzzy_dbg_assert(a) assert((a))
#define muzzy_log_fprintf(f, level, ...)                                        \
  if ((level) <= LOG_LEVEL) {                                                  \
    fprintf((f), "[%d] ", level);                                              \
    fprintf((f), __VA_ARGS__);                                                 \
  }

#define muzzy_dbg(...) muzzy_log_fprintf(stderr, MUZZY_LOG_LEVEL_DBG, __VA_ARGS__)
#define muzzy_warn(...)                                                         \
  muzzy_log_fprintf(stderr, MUZZY_LOG_LEVEL_WARN, __VA_ARGS__)
#define muzzy_info(...)                                                         \
  muzzy_log_fprintf(stderr, MUZZY_LOG_LEVEL_INFO, __VA_ARGS__)
#define muzzy_error(...)                                                        \
  muzzy_log_fprintf(stderr, MUZZY_LOG_LEVEL_ERROR, __VA_ARGS__)

#define muzzy_panic(...)                                                        \
  {                                                                            \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(-1);                                                                  \
  }
#define muzzy_todo(...)                                                         \
  { muzzy_panic(__VA_ARGS__); }

#endif
