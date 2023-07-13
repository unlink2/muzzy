#ifndef RAND_H_
#define RAND_H_

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

#define MUZZY_RAND_ITER_MAX 16

// a rand call gets passed a call id which can be used by the rand function as
// it sees fit and a pointer to a custom object holding information
// about the rng function to call
typedef int64_t (*muzzy_rand)(int id, void *data);

enum muzzy_rands { MUZZY_STDRAND = 0, MUZZY_FRAND };

struct muzzy_rand_cfg {
  enum muzzy_rands kind;
  union {
    struct {
      const char *src_path;
      FILE *fp;
    };
    struct {
      atomic_int iter[MUZZY_RAND_ITER_MAX];
      int next_after;
    };
    atomic_int linear;
    void *custom;
  };
};

struct muzzy_rand_cfg muzzy_rand_cfg_init(void);
struct muzzy_rand_cfg muzzy_rand_cfg_file(const char *path);
struct muzzy_rand_cfg muzzy_rand_cfg_iter(int max);

// regular std-c rand call
// data is rand_cfg
int64_t muzzy_stdrand(int id, void *data);

// read data from file and use them for rand
// data is rand_cfg
int64_t muzzy_frand(int id, void *data);

// data is rand_cfg
int64_t muzzy_lrand(int id, void *data);

int64_t muzzy_iter_rand(int id, void *data);

void muzzy_rand_cfg_free(struct muzzy_rand_cfg *self);
void muzzy_frand_cfg_free(struct muzzy_rand_cfg *self);

#endif
