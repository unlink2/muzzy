#ifndef CONFIG_H_
#define CONFIG_H_

struct muzzy_config {
  const char **args;
};

struct muzzy_config muzzy_config_init(void);

#endif
