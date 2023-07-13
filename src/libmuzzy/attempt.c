#include "libmuzzy/attempt.h"
#include "libmuzzy/buffer.h"
#include "libmuzzy/config.h"
#include "libmuzzy/error.h"
#include "libmuzzy/fuzz.h"
#include "libmuzzy/log.h"
#include "libmuzzy/rand.h"
#include "libmuzzy/vec.h"
#include "libmuzzy/cond.h"
#include "libmuzzy/color.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdatomic.h>

struct muzzy_attempt muzzy_attempt_init(void) {
  struct muzzy_attempt self;
  memset(&self, 0, sizeof(self));

  self.out_to = stdout;
  self.conditions = muzzy_vec_init(sizeof(struct muzzy_cond));

  return self;
}

struct muzzy_attempt_var muzzy_attempt_var_init(size_t args_len) {
  struct muzzy_attempt_var self;
  memset(&self, 0, sizeof(self));

  self.out = muzzy_buffer_init();
  self.buf0 = muzzy_vec_init(sizeof(struct muzzy_buffer));
  self.buf1 = muzzy_vec_init(sizeof(struct muzzy_buffer));

  for (size_t i = 0; i < args_len; i++) {
    // push the right amount of buffers to the target
    struct muzzy_buffer b0 = muzzy_buffer_init();
    struct muzzy_buffer b1 = muzzy_buffer_init();

    muzzy_vec_push(&self.buf0, &b0);
    muzzy_vec_push(&self.buf1, &b1);
  }

  // result buffer used for storage of the actual pointers
  self.args_fuzzed = malloc(sizeof(char *) * (args_len + 1));
  memset(self.args_fuzzed, 0, sizeof(char *) * (args_len + 1));

  return self;
}

struct muzzy_attempt muzzy_attempt_from_cfg(struct muzzy_config *cfg) {
  struct muzzy_attempt self = muzzy_attempt_init();

  self.args = cfg->args;

  muzzy_cond_vec_free(&self.conditions);
  self.conditions = cfg->conditions;

  muzzy_conds_dbg_print(&self.conditions, 0, "", 0);

  size_t args_len = 1; // start at 1 to insert NULL in the end
  const char **arg = cfg->args;
  while (arg[0]) {
    arg++;
    args_len++;
  }

  self.args_len = args_len;

  self.n_runs = cfg->n_runs;
  self.n_threads = cfg->n_threads;
  self.delay_ms = cfg->delay_ms;
  self.rand_cfg = cfg->rand_cfg;
  self.rand = cfg->rand;
  self.dry = cfg->dry;
  self.word_lists = cfg->word_lists;
  self.no_color = cfg->no_color;
  self.no_echo_cmd = cfg->no_echo;
  self.no_cmd_out = cfg->no_cmd_out;
  self.only_ok = cfg->only_ok;

  muzzy_dbg("Delay %d ms\nN-runs: %d\nThreads: %d\nDry: %d\n", self.delay_ms,
            self.n_runs, self.n_threads, self.dry);

  if (strcmp(MUZZY_STDSTREAM_PATH, cfg->out_path) == 0) {
    self.out_to = stdout;
  } else {
    self.out_to = fopen(cfg->out_path, "we");
  }

  self.executable = cfg->prg;

  if (!self.out_to) {
    muzzy_errno();
  }

  return self;
}

struct muzzy_vec *muzzy_attempt_words(int id, struct muzzy_vec *dst,
                                      struct muzzy_vec *args,
                                      struct muzzy_words *wl, muzzy_rand rand,
                                      struct muzzy_rand_cfg *rand_cfg) {
  // fuzz the input
  for (size_t i = 0; i < args->len; i++) {

    if (wl->list.len == 0) {
      muzzy_err_set(MUZZY_ERR_LIST_LEN_ZERO);
      return dst;
    }

    const char **arg = muzzy_vec_get(args, i);
    struct muzzy_buffer *buf = muzzy_vec_get(dst, i);

    if (!buf) {
      struct muzzy_buffer b = muzzy_buffer_init();
      muzzy_vec_push(dst, &b);
      buf = muzzy_vec_get(dst, i);
    }

    muzzy_word_rep_rand(id, *arg, wl->replace, wl->list.data, wl->list.len, -1,
                        rand, rand_cfg, buf);
  }
  return dst;
}

void muzzy_attempt_args_to_buffer(struct muzzy_vec *dst, const char **src) {
  // Note: do not clear this vec! We cant to re-use the buffers!
  // just clear the buffers contained within!
  size_t i = 0;
  while (*src) {
    struct muzzy_buffer *b = muzzy_vec_get(dst, i);

    if (!b) {
      struct muzzy_buffer nb = muzzy_buffer_init();
      muzzy_vec_push(dst, &nb);
      b = muzzy_vec_get(dst, i);
    }

    muzzy_buffer_clear(b);

    size_t len = strlen(*src);
    memcpy(muzzy_buffer_next(b, len), *src, len);
    muzzy_buffer_adv(b, len);
    muzzy_buffer_null_term(b);

    src++;
    i++;
  }
}

const char **muzzy_attempt_to_exec_args(const char **dst,
                                        struct muzzy_vec *src) {
  if (dst == NULL) {
    dst = malloc(sizeof(char *) * (src->len + 1));
  }

  for (size_t i = 0; i < src->len; i++) {
    struct muzzy_buffer *b = muzzy_vec_get(src, i);
    dst[i] = (const char *)b->data;
  }

  dst[src->len] = NULL;
  return dst;
}

const char **muzzy_attempt_fuzz_args(const char **args_fuzzed,
                                     const char **args,
                                     struct muzzy_vec *word_lists,
                                     struct muzzy_vec *buf0,
                                     struct muzzy_vec *buf1, muzzy_rand rand,
                                     struct muzzy_rand_cfg *rand_cfg) {
  struct muzzy_vec *args_buf = buf0;
  struct muzzy_vec *dst_buf = buf1;

  // move source args to buffer
  muzzy_attempt_args_to_buffer(args_buf, args);
  for (size_t i = 0; i < word_lists->len; i++) {
    struct muzzy_words *word_list = muzzy_vec_get(word_lists, i);
    muzzy_attempt_words((int)i, dst_buf, args_buf, word_list, rand, rand_cfg);
    if (muzzy_err()) {
      return args_fuzzed;
    }

    // swap for next iteration
    struct muzzy_vec *tmp = args_buf;
    args_buf = dst_buf;
    dst_buf = tmp;
  }
  // move buffer back to fuzzed args
  args_fuzzed = muzzy_attempt_to_exec_args(args_fuzzed, args_buf);
  return args_fuzzed;
}

int muzzy_attempt_exec(struct muzzy_attempt *self,
                       struct muzzy_attempt_var *var) {
  const char **args_fuzzed = muzzy_attempt_fuzz_args(
      var->args_fuzzed, self->args, &self->word_lists, &var->buf0, &var->buf1,
      self->rand, &self->rand_cfg);
  if (muzzy_err()) {
    return -1;
  }

  int status = 0;
  if (self->dry) {
    const char **arg = args_fuzzed;

    while (*arg) {
      size_t len = strlen(*arg);
      memcpy(muzzy_buffer_next(&var->out, len), *arg, len);
      muzzy_buffer_adv(&var->out, len);
      memcpy(muzzy_buffer_next(&var->out, 1), " ", 1);
      muzzy_buffer_adv(&var->out, 1);
      arg++;
    }
    memcpy(muzzy_buffer_next(&var->out, 1), "\n", 1);
    muzzy_buffer_adv(&var->out, 1);

    muzzy_buffer_null_term(&var->out);
  } else {
    int link[2];
    if (pipe(link) == -1) { // NOLINT
      muzzy_err_set(MUZZY_ERR_PIPE);
      return true;
    }

    pid_t pid = fork();
    if (pid == -1) {
      muzzy_err_set(MUZZY_ERR_FORK);
      return true;
    }

    if (pid == 0) {
      // child proc
      dup2(link[1], STDOUT_FILENO);

      // set up pipes
      close(link[0]);
      close(link[1]);

      execv(self->executable, (char **)var->args_fuzzed);
      // this should never be called!
      muzzy_panic("'%s': No such file or directory\n", // NOLINT
                  self->executable);                   // NOLINT
    } else {
      // parent proc
      close(link[1]);
      // wait for child to exit and obtain exit code
      waitpid(pid, &status, 0);

      int64_t n_read = 0;
      do {
        // read from fd until end or error
        uint8_t *next = muzzy_buffer_next(&var->out, MUZZY_BUF_READ);
        muzzy_dbg_assert(next);

        n_read = read(link[0], next, MUZZY_BUF_READ);
        if (n_read == -1) {
          muzzy_errno();
          return EXIT_FAILURE;
        }

        muzzy_buffer_adv(&var->out, n_read);
      } while (n_read);

      close(link[0]);
    }
  }

  return status;
}

int muzzy_attempt_run_sync(struct muzzy_attempt *self,
                           struct muzzy_attempt_var *var) {
  int act_exit_code = 0;
  atomic_int n_runs = self->n_runs;
  while (n_runs == MUZZY_N_RUNS_INF || n_runs--) {
    muzzy_buffer_clear(&var->out);
    // act
    act_exit_code = muzzy_attempt_exec(self, var);
    if (muzzy_err()) {
      break;
    }

    // condition
    int fd = fileno(self->out_to);
    bool color = isatty(fd) && !self->no_color;
    bool ok = true;
    if (self->conditions.len == 0) {
      var->cond_out[0] = '\0';
    } else if (muzzy_conds_check(&self->conditions, act_exit_code,
                                 (const char *)muzzy_buffer_start(&var->out),
                                 0)) {
      if (color) {
        muzzy_color(var->cond_out, MUZZY_COLOR_GREEN);
      } else {
        var->cond_out[0] = '+';
        var->cond_out[1] = ' ';
      }
      ok = true;
    } else {
      if (color) {
        muzzy_color(var->cond_out, MUZZY_COLOR_RED);
      } else {
        var->cond_out[0] = '-';
        var->cond_out[1] = ' ';
      }
      ok = false;
    }

    if (!self->only_ok || ok) {
      // output
      if (fputs(var->cond_out, self->out_to) == -1) {
        muzzy_errno();
        break;
      }

      if (!self->no_echo_cmd) {
        const char **arg = var->args_fuzzed;
        fputs(">", self->out_to);
        while (*arg) {
          fputs(*arg, self->out_to);
          fputs(" ", self->out_to);
          arg++;
        }
        fputs("\n", self->out_to);
      }

      if (!self->no_cmd_out) {
        if (fwrite(muzzy_buffer_start(&var->out), 1,
                   muzzy_buffer_len(&var->out), self->out_to) == -1) {
          muzzy_errno();
          break;
        }
      }

      if (color) {
        muzzy_color(var->cond_out, MUZZY_COLOR_OFF);
        if (fputs(var->cond_out, self->out_to) == -1) {
          muzzy_errno();
          break;
        }
      }
    }

    if (self->delay_ms) {
      usleep(self->delay_ms);
    }
  }

  muzzy_err_print(stderr);

  return act_exit_code;
}

int muzzy_attempt_run_async(void *data) {
  struct muzzy_attempt_var *var = data;

  return muzzy_attempt_run_sync(var->attempt, var);
}

int muzzy_attempt_run(struct muzzy_attempt *self) {
  if (self->n_threads) {
    int threads_exit_code = 0;

    thrd_t threads[MUZZY_ATTEMPT_THREADS_MAX];
    struct muzzy_attempt_var vars[MUZZY_ATTEMPT_THREADS_MAX];

    // spawn
    for (size_t i = 0; i < self->n_threads; i++) {
      vars[i] = muzzy_attempt_var_init(self->args_len);
      vars[i].attempt = self;

      int rc = thrd_create(&threads[i], muzzy_attempt_run_async, &vars[i]);
      if (rc != thrd_success) {
        muzzy_err_set(MUZZY_ERR_THREAD);
      }
    }

    // join threads
    for (size_t i = 0; i < self->n_threads; i++) {
      int ec = 0;
      thrd_join(threads[i], &ec);
      if (ec) {
        threads_exit_code = ec;
      }
    }

    // free vats
    for (size_t i = 0; i < self->n_threads; i++) {
      muzzy_attempt_var_free(&vars[i]);
    }

    return threads_exit_code;
  } else {
    struct muzzy_attempt_var var = muzzy_attempt_var_init(self->args_len);
    int act_exit_code = muzzy_attempt_run_sync(self, &var);
    muzzy_attempt_var_free(&var);

    return act_exit_code;
  }
}

void muzzy_attempt_free(struct muzzy_attempt *self) {
  muzzy_cond_vec_free(&self->conditions);

  muzzy_rand_cfg_free(&self->rand_cfg);

  for (size_t i = 0; i < self->word_lists.len; i++) {
    muzzy_words_free(muzzy_vec_get(&self->word_lists, i));
  }
  muzzy_vec_free(&self->word_lists);

  if (self->out_to != stdout) {
    fclose(self->out_to);
  }

  const char **arg = self->args;
  while (arg && *arg) {
    free((void *)*arg);
    arg++;
  }
  free(self->args);

  free((void *)self->executable);
}

void muzzy_attempt_var_free(struct muzzy_attempt_var *self) {
  muzzy_buf_vec_free(&self->buf0);
  muzzy_buf_vec_free(&self->buf1);

  muzzy_buffer_free(&self->out);
  free(self->args_fuzzed);
}
