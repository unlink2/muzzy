#include "arg.h"
#include "libmuzzy/attempt.h"
#include "libmuzzy/buffer.h"
#include "libmuzzy/config.h"
#include "libmuzzy/fuzz.h"
#include "libmuzzy/log.h"
#include "libmuzzy/rand.h"
#include <argtable2.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct muzzy_config muzzy_args_to_config(int argc, char **argv) {
  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;

  struct arg_file *words = NULL;
  struct arg_str *replace = NULL;
  struct arg_lit *no_sh = NULL;
  struct arg_int *delay_ms = NULL;
  struct arg_lit *stdrand = NULL;
  struct arg_int *seed_rand = NULL;
  struct arg_int *n_runs = NULL;
  struct arg_file *rand_file = NULL;
  struct arg_lit *dry = NULL;

  struct arg_file *output = NULL;

  struct arg_str *condition = NULL;

  struct arg_str *command = NULL;

  // arg end stores errors
  struct arg_end *end = NULL;

  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
      verb = arg_litn("v", "verbose", 0, MUZZY_LOG_LEVEL_DBG, "verbose output"),

      words = arg_file1("w", "words", "word list", "Read a word list file"),
      replace = arg_str0(
          "r", "replace", "replace string",
          "Add a replace string. These are read in order for each word list."),

      no_sh =
          arg_lit0(NULL, "nosh",
                   "Execute command directly instead of through '/bin/sh -c'."),
      delay_ms = arg_int0(NULL, "delay", "MS", "Delay between attempts in ms."),
      stdrand = arg_lit0(NULL, "stdrand", "Use built-in rand."),
      seed_rand = arg_int0(NULL, "seed", "INT", "Seed the built-in rand."),
      n_runs = arg_int0("n", "nruns", "INT", "Amount of attempts to run"),
      rand_file = arg_file0(NULL, "frand", "FILE",
                            "The file to read from as a source of randomness. "
                            "Defaults to '/dev/urandom'."),

      dry = arg_lit0(NULL, "dry", "Run without executing an actual command"),
      output = arg_file0("o", "output", "FILE", "Output file"),

      condition = arg_str0("c", "cond", "cond", "Condition to check for"),

      command = arg_strn(NULL, NULL, "", 1, 4096, "The command to execute"),
      end = arg_end(20),
  };

  // output params
  char progname[] = "muzzy";
  char short_desc[] =
      "muzzy - mini fuzzer\nWritten by Lukas Krickl <lukas@krickl.dev>";

  // version info
  int version_major = 0;
  int version_minor = 0;
  int version_patch = 1;

  int nerrors = arg_parse(argc, argv, argtable);
  int exitcode = 0;

  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf("%s\n\n", short_desc);
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    goto exit;
  }

  if (version->count) {
    printf("%s version %d.%d.%d\n", progname, version_major, version_minor,
           version_patch);
    goto exit;
  }

  if (verb->count > 0) {
    muzzy_log_init(verb->count);
  } else {
    muzzy_log_init(MUZZY_LOG_LEVEL_ERROR);
  }

  if (nerrors > 0) {
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  struct muzzy_config cfg = muzzy_config_init();

  // map args to cfg data here

  // map command to run here
  if (no_sh->count) {
    // use env as arg value
    size_t len = sizeof(char *) * (command->count + 3);
    cfg.args = malloc(len);
    memset(cfg.args, 0, len);
    for (size_t i = 0; i < command->count; i++) {
      cfg.args[i] = strdup(command->sval[i]);
    }
    cfg.prg = strdup(MUZZY_DEFAULT_EXECUTABLE_ENV);
  } else {
    // use sh and combine all args into a single statement
    size_t len = sizeof(char *) * 4; // sh -c 'command str' NULL
    cfg.args = malloc(len);
    memset(cfg.args, 0, len);

    cfg.args[0] = strdup(command->sval[0]);
    cfg.args[1] = strdup(MUZZY_DEFAULT_EXECUTABLE_ARG1);
    cfg.args[2] =
        (const char *)muzzy_buffer_cat(' ', command->sval, command->count);

    cfg.prg = strdup(MUZZY_DEFAULT_EXECUTABLE);
  }

  // dbg output for args and prg
  const char **arg = cfg.args;
  muzzy_dbg("Prg: '%s' ", cfg.prg);
  while (arg[0]) {
    muzzy_dbg(" %s", arg[0]);
    arg++;
  }
  muzzy_dbg("\n");

  // set up word lists
  for (size_t i = 0; i < words->count; i++) {
    const char *rep_word = MUZZY_DEFAULT_REPLACE_WORD;
    if (i < replace->count) {
      rep_word = replace->sval[i];
    }

    muzzy_dbg("Reading word list from '%s' with keyword: '%s'\n",
              words->filename[i], rep_word);
    struct muzzy_words n = muzzy_words_from_file(words->filename[i], rep_word);
    muzzy_vec_push(&cfg.word_lists, &n);
  }

  if (delay_ms->count) {
    cfg.delay_ms = delay_ms->ival[delay_ms->count - 1];
  } else {
    cfg.delay_ms = MUZZY_DEFAULT_DELAY_MS;
  }

  if (n_runs->count) {
    cfg.n_runs = n_runs->ival[n_runs->count - 1];
  } else {
    cfg.n_runs = MUZZY_N_RUNS_INF;
  }

  if (stdrand->count) {
    cfg.rand = muzzy_stdrand;
    cfg.rand_cfg = muzzy_rand_cfg_init();
  } else {
    cfg.rand = muzzy_frand;

    if (rand_file->count) {
      cfg.rand_cfg =
          muzzy_rand_cfg_file(rand_file->filename[rand_file->count - 1]);
    } else {
      cfg.rand_cfg = muzzy_rand_cfg_file(MUZZY_DEFAULT_RAND_FILE);
    }
  }

  if (seed_rand->count) {
    srand(seed_rand->ival[seed_rand->count - 1]);
  }

  cfg.dry = dry->count > 0;

  if (output->count) {
    cfg.out_path = output->filename[output->count - 1];
  } else {
    cfg.out_path = MUZZY_STDSTREAM_PATH;
  }

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return cfg;
exit:
  exit(exitcode); // NOLINT
}
