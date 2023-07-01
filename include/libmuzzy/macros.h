#ifndef MACROS_H_
#define MACROS_H_

#define UNUSED(...) (void)(__VA_ARGS__)
#define memclear(v) memset(&(v), 0, sizeof(v));

#endif
