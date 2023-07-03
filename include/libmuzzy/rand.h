#ifndef RAND_H_
#define RAND_H_

#include <stdint.h>

typedef int64_t (*muzzy_next_rand)(void *data);

// regular std-c rand call
int64_t muzzy_rand(void *data);

// read data from file and use them for rand
int64_t muzzy_frand(void *data);

#endif
