#ifndef RAND_H_
#define RAND_H_

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

typedef int64_t (*muzzy_rand)(int id, void *data);

enum muzzy_rands { MUZZY_STDRAND = 0, MUZZY_FRAND };

struct muzzy_rand_cfg {
  enum muzzy_rands kind;
  union {
    struct {
      const char *src_path;
      FILE *fp;
    };
    atomic_int linear;
    void *custom;
  };
};

struct muzzy_rand_cfg muzzy_rand_cfg_init(void);
struct muzzy_rand_cfg muzzy_rand_cfg_file(const char *path);

// regular std-c rand call
// data is rand_cfg
int64_t muzzy_stdrand(int id, void *data);

// read data from file and use them for rand
// data is rand_cfg
int64_t muzzy_frand(int id, void *data);

// data is rand_cfg
int64_t muzzy_lrand(int id, void *data);

void muzzy_rand_cfg_free(struct muzzy_rand_cfg *self);
void muzzy_frand_cfg_free(struct muzzy_rand_cfg *self);

#endif
