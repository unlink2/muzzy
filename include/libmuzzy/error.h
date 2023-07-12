#ifndef ERROR_H_
#define ERROR_H_

#include "libmuzzy/macros.h"
#include <stddef.h>
#include <errno.h>
#include <stdio.h>

/**
 * Error is a error type struct that provides
 * an errno-style interface.
 * It provides a generic message location as well as a details buffer
 * where additional error information can  be stored.
 * Both of those locations are thread local and therefore
 * are OK to overwirte at any point.
 * It is up to the caller to handle and read the buffers if
 * an error is returned. Do not rely on the buffer not being overwirtten after
 * making a new call! The error info struct should be included in structs.
 */

#define MUZZY_ERR_MSG_LEN 1024
#define MUZZY_ERR_DETAIL_LEN 1024

// Possible error types.
// Some may or may not require special error info
enum muzzy_error {
  MUZZY_OK = 0,
  MUZZY_ERR_LIST_LEN_ZERO,
  // err_detail points to expression that failed
  MUZZY_ERR_COND_PARSE,
  // errno is stored in err_detail
  MUZZY_ERR_PIPE,
  MUZZY_ERR_FORK,
  MUZZY_ERRNO
};

const char *muzzy_err_to_str(enum muzzy_error self);
void muzzy_err_print(FILE *f);

#define muzzy_err_msg(...) sprintf(muzzy_err_msg_ptr(), __VA_ARGS__);

#define muzzy_errno()                                                          \
  {                                                                            \
    muzzy_err_set(MUZZY_ERRNO);                                                \
    muzzy_err_detail(&errno, sizeof(errno));                                   \
  }

#define muzzy_err_detail(data, len) memcpy(muzzy_err_details_ptr(), data, len);

// sets err to a value
// to indicate a failure in a call
void muzzy_err_set(enum muzzy_error err);

// return error code and resets value
// to OK
enum muzzy_error muzzy_err(void);
void muzzy_err_rest(void);

// the error message is in practice a thread local
// string buffer where errors may write specific information
// about the kind of failure. It should only ever be obtained
// via this function.
// This of course means that subsequent errors may overwrite
// the previous error message! Ensure to handle errors in a timely manner
// to avoid losing error information
char *muzzy_err_msg_ptr(void);
size_t muzzy_err_msg_len(void);

// Error details can be used to store metadata about the error
// The data type depends on the error kind and should be documented above
// if available
void *muzzy_err_details_ptr(void);
size_t muzzy_err_details_len(void);

// bail on error macro
#define muzzy_ok_or(err, ret)                                                  \
  {                                                                            \
    if ((err)) {                                                               \
      return (ret);                                                            \
    }                                                                          \
  }

// optional struct
#define muzzy_some_or(optional, ret)                                           \
  {                                                                            \
    if ((optional)) {                                                          \
      return (ret);                                                            \
    }                                                                          \
  }

#endif
