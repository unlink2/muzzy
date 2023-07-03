#ifndef BUFFER_H_
#define BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#define MUZZY_BUFFER_INITIAL_LEN 128

/**
 * Simple expanding string buffer
 */
struct muzzy_buffer {
  uint8_t *data;
  size_t index;
  size_t max_len;
};

struct muzzy_buffer muzzy_buffer_init(void);

// reserve the next 'len' bytes
// for use - if the buffer does not have enough data at this point
// it will resize to fit twice the requested length
// and return the apropriate pointer
uint8_t *muzzy_buffer_next(struct muzzy_buffer *self, size_t len);

// advance index by n bytes
void muzzy_buffer_adv(struct muzzy_buffer *self, size_t n);

void muzzy_buffer_clear(struct muzzy_buffer *self);

// returns the start address of the current buffer ptr
const uint8_t *muzzy_buffer_start(struct muzzy_buffer *self);
size_t muzzy_buffer_len(struct muzzy_buffer *self);

// this call NULL terminates the buffer by appending \0
// This will *not* advance the buffer. it simply ensures
// termination
void muzzy_buffer_null_term(struct muzzy_buffer *self);

// moves the data owned by the buffer out of the buffer
// and guarantees to clean up the remaining data the buffer might use
// using the buffer after this call is ub.
// The caller is responsible for calling free() on the returned
// data
uint8_t *muzzy_buffer_move(struct muzzy_buffer *self);

uint8_t *muzzy_buffer_cat(char separator, const char **strings, size_t len);

void muzzy_buffer_free(struct muzzy_buffer *self);

#endif
