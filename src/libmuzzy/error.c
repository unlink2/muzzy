#include <libmuzzy/macros.h>
#include <libmuzzy/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char ERR_MESSAGE[MUZZY_ERR_MSG_LEN];
_Thread_local char ERR_DETAILS[MUZZY_ERR_DETAIL_LEN];
_Thread_local enum muzzy_error MUZZY_ERR;

const char *muzzy_err_to_str(enum muzzy_error self) {
  switch (self) {
  case MUZZY_OK:
    return "OK";
  case MUZZY_ERRNO:
    return strerror(errno); // NOLINT
  }

  return "Unknown Error";
}

void muzzy_err_set(enum muzzy_error err) { MUZZY_ERR = err; }

enum muzzy_error muzzy_err(void) {
  enum muzzy_error latest = MUZZY_ERR;
  return latest;
}

void muzzy_err_reset(void) { MUZZY_ERR = MUZZY_OK; }

char *muzzy_err_msg_ptr(void) { return ERR_MESSAGE; }

size_t muzzy_err_msg_len(void) { return MUZZY_ERR_MSG_LEN; }

void *muzzy_err_details_ptr(void) { return ERR_DETAILS; }

size_t muzzy_err_details_len(void) { return MUZZY_ERR_DETAIL_LEN; }
