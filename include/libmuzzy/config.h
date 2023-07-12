#ifndef CONFIG_H_
#define CONFIG_H_

#include "libmuzzy/macros.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"

#define MUZZY_STDSTREAM_PATH "-"

struct muzzy_config {
  const char **args;
  const char *prg;

  muzzy_rand rand;
  struct muzzy_rand_cfg rand_cfg;

  struct muzzy_vec word_lists;
  struct muzzy_vec conditions;
  bool no_sh; // run command using /usr/bin/env instead of sh -c
  bool dry;
  int32_t delay_ms;
  int32_t n_runs;
  bool no_color;
  bool no_echo;
  bool no_cmd_out;
  bool only_ok;

  const char *out_path;
};

struct muzzy_config muzzy_config_init(void);

void muzzy_config_free(struct muzzy_config *self);

#endif
