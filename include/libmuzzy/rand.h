#ifndef RAND_H_
#define RAND_H_

#include <stdint.h>
#include <stdio.h>

typedef int64_t (*muzzy_rand)(void *data);

struct muzzy_rand_cfg {
  union {
    struct {
      const char *src_path;
      FILE *fp;
    };
    void *custom;
  };
};

struct muzzy_rand_cfg muzzy_rand_cfg_init(void);
struct muzzy_rand_cfg muzzy_rand_cfg_file(const char *path);

// regular std-c rand call
int64_t muzzy_stdrand(void *data);

// read data from file and use them for rand
int64_t muzzy_frand(void *data);

void muzzy_rand_cfg_free(struct muzzy_rand_cfg *self);
void muzzy_frand_cfg_free(struct muzzy_rand_cfg *self);

#endif
